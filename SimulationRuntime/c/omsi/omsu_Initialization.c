/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THE BSD NEW LICENSE OR THE
 * GPL VERSION 3 LICENSE OR THE OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs: http://www.openmodelica.org or
 * http://www.ida.liu.se/projects/OpenModelica, and in the OpenModelica
 * distribution. GNU version 3 is obtained from:
 * http://www.gnu.org/copyleft/gpl.html. The New BSD License is obtained from:
 * http://www.opensource.org/licenses/BSD-3-Clause.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, EXCEPT AS
 * EXPRESSLY SET FORTH IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE
 * CONDITIONS OF OSMC-PL.
 *
 */

/*
 * Author name [e-mail]
 * This file defines functions for the FMI used via the OpenModelica Simulation
 * Interface (OMSI). These functions are used for instantiation and initialization
 * of the FMU.
 */

#include "omsu_Initialization.h"

fmi2Component omsi_instantiate(fmi2String instanceName,
                               fmi2Type   fmuType,
                               fmi2String fmuGUID,
                               fmi2String fmuResourceLocation,
                               const fmi2CallbackFunctions* functions,
                               fmi2Boolean                  visible,
                               fmi2Boolean                  loggingOn)
{
  loggingOn = fmi2True;
  osu_t *OSU;

  /* check all input arguments */
  /* ignoring arguments: fmuResourceLocation, visible */
  if (!functions->logger) {
    return NULL;
  }
  if (!functions->allocateMemory || !functions->freeMemory) {
    functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error", "fmi2Instantiate: Missing callback function.");
    return NULL;
  }
  if (!instanceName || strlen(instanceName) == 0) {
    functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error", "fmi2Instantiate: Missing instance name.");
    return NULL;
  }


  OSU = (osu_t *)functions->allocateMemory(1, sizeof(osu_t));
  OSU->fmiCallbackFunctions = functions;

  if (OSU) {
    DATA* data = (DATA *)functions->allocateMemory(1, sizeof(DATA));
    MODEL_DATA* modelData = (MODEL_DATA *)functions->allocateMemory(1, sizeof(MODEL_DATA));
    SIMULATION_INFO* simInfo = (SIMULATION_INFO *)functions->allocateMemory(1, sizeof(SIMULATION_INFO));
    threadData_t *threadData = (threadData_t *)functions->allocateMemory(1, sizeof(threadData_t));
    omsi_functions_t*  osu_functions = (omsi_functions_t*)functions->allocateMemory(1, sizeof(omsi_functions_t));


    OSU->instanceName = (fmi2String)functions->allocateMemory(1 + strlen(instanceName), sizeof(char));
    strcpy((char*)OSU->instanceName, (const char*)instanceName);


    data->modelData = modelData;
    data->simulationInfo = simInfo;

    memset(threadData, 0, sizeof(threadData_t));
    OSU->osu_functions = osu_functions;
    OSU->threadData = threadData;
    OSU->old_data = data;
    if (!OSU->old_data) {
      OSU->fmiCallbackFunctions->logger(OSU->fmiCallbackFunctions->componentEnvironment, instanceName, fmi2Error, "error", "fmi2Instantiate: Could not initialize the global data structure file.");
      return NULL;
    }
    OSU->type = fmuType;

    // set all categories to on or off. fmi2SetDebugLogging should be called to choose specific categories.
    {
      int i;
      for (i = 0; i < NUMBER_OF_CATEGORIES; i++) {
        OSU->logCategories[i] = loggingOn;
      }
    }
  }

  /* initialize modelData */
  omsic_model_setup_data(OSU);
  initializeDataStruc(OSU->old_data, OSU->threadData);

  /* create init.xml filename  */
  char* initFilename = (fmi2String)functions->allocateMemory(20 + strlen(instanceName) + strlen(fmuResourceLocation), sizeof(char));
  sprintf(initFilename, "%s/%s_init.xml", fmuResourceLocation, instanceName);
  read_input_xml(OSU->old_data->modelData, OSU->old_data->simulationInfo, initFilename);

  OSU->GUID = fmuGUID;
  if (strcmp(fmuGUID, OSU->old_data->modelData->modelGUID) != 0) {
    OSU->fmiCallbackFunctions->logger(OSU->fmiCallbackFunctions->componentEnvironment, instanceName, fmi2Error, "error", "fmi2Instantiate: Wrong GUID %s. Expected %s.", fmuGUID, OSU->old_data->modelData->modelGUID);    return NULL;
  }

  /* read input vars */
  /* input_function(comp->fmuData); */
  /* allocate memory for non-linear system solvers */
  initializeNonlinearSystems(OSU->old_data, OSU->threadData);
  /* allocate memory for non-linear system solvers */
  initializeLinearSystems(OSU->old_data, OSU->threadData);
  /* allocate memory for mixed system solvers */
  initializeMixedSystems(OSU->old_data, OSU->threadData);
  /* allocate memory for state selection */
  initializeStateSetJacobians(OSU->old_data, OSU->threadData);


  FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2Instantiate: GUID=%s", fmuGUID)

  OSU->state = modelInstantiated;
  return OSU;
}


fmi2Status omsi_enter_initialization_mode(fmi2Component c)
{
	osu_t* OSU = (osu_t *)c;
	threadData_t *threadData = OSU->threadData;
	double nextSampleEvent;

	threadData->currentErrorStage = ERROR_SIMULATION;
	if (invalidState(OSU, "fmi2EnterInitializationMode", modelInstantiated, ~0))
		return fmi2Error;

	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2EnterInitializationMode...")
	/* set zero-crossing tolerance */
	setZCtol(OSU->tolerance);

	//setStartValues(OSU);
	copyStartValuestoInitValues(OSU->old_data);

	/* try */
	MMC_TRY_INTERNAL(simulationJumpBuffer)

	if (initialization(OSU->old_data, OSU->threadData, "", "", 0.0)) {
		OSU->state = modelError;
		FILTERED_LOG(OSU, fmi2Error, LOG_FMI2_CALL, "fmi2EnterInitializationMode: failed")
		return fmi2Error;
		}
	else
	{
		/*TODO: Simulation stop time is need to calculate in before hand all sample events
				  We shouldn't generate them all in beforehand */
		initSample(OSU->old_data, OSU->threadData, OSU->old_data->localData[0]->timeValue, 100 /*should be stopTime*/);
		#if !defined(OMC_NDELAY_EXPRESSIONS) || OMC_NDELAY_EXPRESSIONS>0
			initDelay(OSU->old_data, OSU->old_data->localData[0]->timeValue);
		#endif
		/* due to an event overwrite old values */
		overwriteOldSimulationData(OSU->old_data);

		OSU->eventInfo.terminateSimulation = fmi2False;
		OSU->eventInfo.valuesOfContinuousStatesChanged = fmi2True;

		/* Get next event time (sample calls)*/
		nextSampleEvent = 0;
		nextSampleEvent = getNextSampleTimeFMU(OSU->old_data);
		if (nextSampleEvent == -1) {
			OSU->eventInfo.nextEventTimeDefined = fmi2False;
		} else {
			OSU->eventInfo.nextEventTimeDefined = fmi2True;
			OSU->eventInfo.nextEventTime = nextSampleEvent;
			fmi2EventUpdate(OSU, &(OSU->eventInfo));
		}
		OSU->state = modelInitializationMode;
		FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2EnterInitializationMode: succeed")
		return fmi2OK;
	}

	/* catch */
	MMC_CATCH_INTERNAL(simulationJumpBuffer)

	FILTERED_LOG(OSU, fmi2Error, LOG_FMI2_CALL, "fmi2EnterInitializationMode: terminated by an assertion.")
	return fmi2Error;
}


fmi2Status omsi_exit_initialization_mode(fmi2Component c)
{
	osu_t* OSU = (osu_t *)c;
	if (invalidState(OSU, "fmi2ExitInitializationMode", modelInitializationMode, ~0))
		return fmi2Error;
	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2ExitInitializationMode...")

	OSU->state = modelEventMode;
	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2ExitInitializationMode: succeed")
		return fmi2OK;
}


fmi2Status omsi_setup_experiment(fmi2Component c, fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime)
{
	osu_t* OSU = (osu_t *)c;

	if (invalidState(OSU, "fmi2SetupExperiment", modelInstantiated, ~0))
		return fmi2Error;

	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2SetupExperiment: toleranceDefined=%d tolerance=%g startTime=%g stopTimeDefined=%d stopTime=%g", toleranceDefined, tolerance,
	  startTime, stopTimeDefined, stopTime)

	OSU->toleranceDefined = toleranceDefined;
	OSU->tolerance = tolerance;
	OSU->startTime = startTime;
	OSU->stopTimeDefined = stopTimeDefined;
	OSU->stopTime = stopTime;
	return fmi2OK;
}


void omsi_free_instance(fmi2Component c)
{
	osu_t* OSU = (osu_t *)c;
	int meStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError;
	int csStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError;

	if (invalidState(OSU, "fmi2FreeInstance", meStates, csStates))
	return;

	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2FreeInstance")

	/* free simuation data */
	OSU->fmiCallbackFunctions->freeMemory(OSU->old_data->modelData);
	OSU->fmiCallbackFunctions->freeMemory(OSU->old_data->simulationInfo);

	/* free fmuData */
	OSU->fmiCallbackFunctions->freeMemory(OSU->threadData);
	OSU->fmiCallbackFunctions->freeMemory(OSU->old_data);
	/* free instanceName & GUID */
	if (OSU->instanceName) OSU->fmiCallbackFunctions->freeMemory((void*)OSU->instanceName);
	if (OSU->GUID) OSU->fmiCallbackFunctions->freeMemory((void*)OSU->GUID);
	/* free comp */
	OSU->fmiCallbackFunctions->freeMemory(OSU);
}


fmi2Status omsi_reset(fmi2Component c)
{
	osu_t* OSU = (osu_t *)c;
	if (invalidState(OSU, "fmi2Reset", modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0))
		return fmi2Error;
	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2Reset")

	if (OSU->state & modelTerminated) {
	  /* initialize modelData */
	  omsic_model_setup_data(OSU);
		initializeDataStruc(OSU->old_data, OSU->threadData);
	}
	/* reset the values to start */
	//setDefaultStartValues(OSU);
	setAllVarsToStart(OSU->old_data);
	setAllParamsToStart(OSU->old_data);

	OSU->state = modelInstantiated;
	return fmi2OK;
}


fmi2Status omsi_terminate(fmi2Component c)
{
	osu_t* OSU = (osu_t *)c;
	if (invalidState(OSU, "fmi2Terminate", modelEventMode|modelContinuousTimeMode, ~0))
		return fmi2Error;
	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2Terminate")

	/* call external objects destructors */
	OSU->osu_functions->callExternalObjectDestructors(OSU->old_data, OSU->threadData);
	#if !defined(OMC_NUM_NONLINEAR_SYSTEMS) || OMC_NUM_NONLINEAR_SYSTEMS>0
	/* free nonlinear system data */
	freeNonlinearSystems(OSU->old_data, OSU->threadData);
	#endif
	#if !defined(OMC_NUM_MIXED_SYSTEMS) || OMC_NUM_MIXED_SYSTEMS>0
	/* free mixed system data */
	freeMixedSystems(OSU->old_data, OSU->threadData);
	#endif
	#if !defined(OMC_NUM_LINEAR_SYSTEMS) || OMC_NUM_LINEAR_SYSTEMS>0
	/* free linear system data */
	freeLinearSystems(OSU->old_data, OSU->threadData);
	#endif
	#if !defined(OMC_NO_STATESELECTION)
	/* free stateset data */
	freeStateSetData(OSU->old_data);
	#endif
	/* free data struct */
	deInitializeDataStruc(OSU->old_data);

	OSU->state = modelTerminated;
	return fmi2OK;
}
