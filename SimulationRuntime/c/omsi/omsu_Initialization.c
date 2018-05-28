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
 * This file defines functions for the FMI used via the OpenModelica Simulation
 * Interface (OMSI). These functions are used for instantiation and initialization
 * of the FMU.
 */

#include "omsu_Initialization.h"

/*
 * Allocates memory for the Openmodelica Simulation Unit and initializes it.
 */
fmi2Component omsi_instantiate(fmi2String                   instanceName,
                               fmi2Type                     fmuType,
                               fmi2String                   fmuGUID,
                               fmi2String                   fmuResourceLocation,
                               const fmi2CallbackFunctions* functions,
                               fmi2Boolean                  visible,
                               fmi2Boolean                  loggingOn)
{
  int i;
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
  if (!fmuGUID || strlen(fmuGUID) == 0) {
    functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error", "fmi2Instantiate: Missing GUID.");
    return NULL;
  }

  /* allocate memory for Openmodelica Simulation Unit */
  OSU = functions->allocateMemory(1, sizeof(osu_t));
  if (!OSU) {
    functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error", "fmi2Instantiate: Not enough memory.");
    return NULL;
  }

  /* read xml filename  */
  char* initFilename = functions->allocateMemory(20 + strlen(instanceName) + strlen(fmuResourceLocation), sizeof(char));
  sprintf(initFilename, "%s/%s_init.xml", fmuResourceLocation, instanceName);
  read_input_xml(OSU->osu_data, initFilename, fmuGUID, instanceName, functions);

  if (!omsu_allocate_osu_data(OSU->osu_data, functions, strlen(fmuGUID))) {     // ToDo: needs some information beforehand
    functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error", "fmi2Instantiate: Not enough memory.");
    omsu_free_osu_data(OSU->osu_data, functions);
    reutrn NULL;
  }
  OSU->osu_functions = functions->allocateMemory(1, sizeof(omsi_functions_t));
  OSU->instanceName = functions->allocateMemory(1 + strlen(instanceName), sizeof(char));
  OSU->vrStates = functions->allocateMemory(1, sizeof(fmi2ValueReference));
  OSU->vrStatesDerivatives = functions->allocateMemory(1, sizeof(fmi2ValueReference));
  OSU->fmiCallbackFunctions = functions->allocateMemory(1, sizeof(fmi2CallbackFunctions));

  // ToDo: implement error case out of memory for OSU->...



  /* initialize OSU */
  omsic_model_setup_data(OSU);      // ToDo: implement, set model data, experiment data
  // set all categories to on or off. fmi2SetDebugLogging should be called to choose specific categories.
  for (i = 0; i < NUMBER_OF_CATEGORIES; i++) {
    OSU->logCategories[i] = loggingOn;
  }
  OSU->loggingOn = loggingOn;
  OSU->GUID = fmuGUID;
  strcpy((char*)OSU->instanceName, (const char*)instanceName);      // ToDo: Do we need to allocate memory for string?
  OSU->type = fmuType;
  OSU->fmiCallbackFunctions = functions;

  /* setup simulation data with default start values */
  OSU->osu_functions->setupStartValues(OSU->osu_data);       // ToDo: implement. Probably pointer to generated function to set up default start data

  OSU->state = modelInstantiated;
  FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2Instantiate: GUID=%s", fmuGUID)
  return OSU;
}

/*
 * Informs the Openmodelica Simulation Unit to enter the initialization mode.
 * Initializes simulation data.
 */
fmi2Status omsi_enter_initialization_mode(fmi2Component c)
{
    osu_t* OSU = (osu_t *)c;
//    threadData_t *threadData = OSU->threadData;
//    double nextSampleEvent;

//    threadData->currentErrorStage = ERROR_SIMULATION;
    if (invalidState(OSU, "fmi2EnterInitializationMode", modelInstantiated, ~0))
        return fmi2Error;

    FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2EnterInitializationMode...")
    /* set zero-crossing tolerance */
    setZCtol(OSU->tolerance);       // ToDo: is this redundant information? Should be in model_info

    //setStartValues(OSU);
    copyStartValuestoInitValues(OSU->osu_data);

    omsu_initialization(OSU->osu_data);     // ToDo: implement, should set simulation data

//    /* try */
//    MMC_TRY_INTERNAL(simulationJumpBuffer)
//
//    if (initialization(OSU->old_data, OSU->threadData, "", "", 0.0)) {
//        OSU->state = modelError;
//        FILTERED_LOG(OSU, fmi2Error, LOG_FMI2_CALL, "fmi2EnterInitializationMode: failed")
//        return fmi2Error;
//        }
//    else {
//        /*TODO: Simulation stop time is need to calculate in before hand all sample events
//                  We shouldn't generate them all in beforehand */
//        initSample(OSU->old_data, OSU->threadData, OSU->old_data->localData[0]->timeValue, 100 /*should be stopTime*/);
//        #if !defined(OMC_NDELAY_EXPRESSIONS) || OMC_NDELAY_EXPRESSIONS>0
//            initDelay(OSU->old_data, OSU->old_data->localData[0]->timeValue);
//        #endif
//        /* due to an event overwrite old values */
//        overwriteOldSimulationData(OSU->old_data);
//
//        OSU->eventInfo.terminateSimulation = fmi2False;
//        OSU->eventInfo.valuesOfContinuousStatesChanged = fmi2True;
//
//        /* Get next event time (sample calls)*/
//        nextSampleEvent = 0;
//        nextSampleEvent = getNextSampleTimeFMU(OSU->old_data);
//        if (nextSampleEvent == -1) {
//            OSU->eventInfo.nextEventTimeDefined = fmi2False;
//        } else {
//            OSU->eventInfo.nextEventTimeDefined = fmi2True;
//            OSU->eventInfo.nextEventTime = nextSampleEvent;
//            fmi2EventUpdate(OSU, &(OSU->eventInfo));
//        }
//        OSU->state = modelInitializationMode;
//        FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2EnterInitializationMode: succeed")
//        return fmi2OK;
//    }
//
//    /* catch */
//    MMC_CATCH_INTERNAL(simulationJumpBuffer)

    FILTERED_LOG(OSU, fmi2Error, LOG_FMI2_CALL, "fmi2EnterInitializationMode: terminated by an assertion.")
    return fmi2Error;
}

/*
 * Informs the Openmodelica Simulation Unit to exit initialization mode.
 */
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

/*
 * Setup experiment data for the Openmodelica Simulation Unit.
 * ToDo: Actually overwrites already set values from read_input_xml
 */
fmi2Status omsi_setup_experiment(fmi2Component c, fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime)
{
    osu_t* OSU = (osu_t *)c;

    if (invalidState(OSU, "fmi2SetupExperiment", modelInstantiated, ~0))
        return fmi2Error;

    FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL,
        "fmi2SetupExperiment: toleranceDefined=%d tolerance=%g startTime=%g stopTimeDefined=%d stopTime=%g",
        toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime)

    if (toleranceDefined) {
        OSU->osu_data->experiment->tolerance = tolerance;
    }
    else {
        OSU->osu_data->experiment->tolerance = 1e-5;        // ToDo: some default value
    }
    OSU->osu_data->experiment->start_time = startTime;
    if (stopTimeDefined) {
        OSU->osu_data->experiment->stop_time = stopTime;
    }
    else {
        OSU->osu_data->experiment->stop_time = startTime+1;     // ToDo: some default value
    }

    // ToDo: solver_name, num_outputs, step_size not defined

    return fmi2OK;
}

/*
 * Frees all allocated memory for the Openmodelica Simulation Unit.
 * Does nothing if a null pointer is provided.
 */
void omsi_free_instance(fmi2Component c)
{
    if (c==NULL) {
        return;
    }

    osu_t* OSU = (osu_t *)c;
    int meStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError;
    int csStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError;

    if (invalidState(OSU, "fmi2FreeInstance", meStates, csStates))
        return;

    FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2FreeInstance")

    /* free OSU data */
    omsu_free_osu_data(OSU->osu_data, functions);     // ToDo: implement, should free everything inside osu_data
    OSU->fmiCallbackFunctions->freeMemory(OSU->osu_data);
    // ToDo: free everithing inside osu_functions
    OSU->fmiCallbackFunctions->freeMemory(OSU->osu_functions);

    /* free fmuData */
    OSU->fmiCallbackFunctions->freeMemory(OSU->threadData);
    OSU->fmiCallbackFunctions->freeMemory(OSU->old_data);
    /* free instanceName & GUID */
    if (OSU->instanceName) OSU->fmiCallbackFunctions->freeMemory((void*)OSU->instanceName);
    if (OSU->GUID) OSU->fmiCallbackFunctions->freeMemory((void*)OSU->GUID);
    /* free comp */
    OSU->fmiCallbackFunctions->freeMemory(OSU);
}

/*
 * Resets the Openmodelica Simulation Unit.
 */
fmi2Status omsi_reset(fmi2Component c)
{
    osu_t* OSU = (osu_t *)c;
    if (invalidState(OSU, "fmi2Reset", modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0))
        return fmi2Error;
    FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2Reset")

    if (OSU->state & modelTerminated) {
      /* initialize OSU */
      omsic_model_setup_data(OSU);
    }
    /* reset the values to start */
    setDefaultStartValues(OSU);     // ToDo: implement

    OSU->state = modelInstantiated;
    return fmi2OK;
}

/*
 * Informs that the simulation run is terminated.
 */
fmi2Status omsi_terminate(fmi2Component c)
{
    osu_t* OSU = (osu_t *)c;
    if (invalidState(OSU, "fmi2Terminate", modelEventMode|modelContinuousTimeMode, ~0))
        return fmi2Error;
    FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2Terminate")

    OSU->state = modelTerminated;
    return fmi2OK;
}
