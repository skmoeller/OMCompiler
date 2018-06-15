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

#include "omsu/omsu_Initialization.h"

/*
 * Allocates memory for the Openmodelica Simulation Unit and initializes it.
 */
osu_t* omsi_instantiate(omsi_string                    instanceName,
                        omsu_type                      fmuType,
                        omsi_string                    fmuGUID,
                        omsi_string                    fmuResourceLocation,
                        const omsi_callback_functions* functions,
                        omsi_bool                      __attribute__((unused)) visible,
                        omsi_bool                      loggingOn)
{
    osu_t *OSU;

    /* check all input arguments */
    /* ignoring arguments: fmuResourceLocation, visible */
    if (!functions->logger) {
        return NULL;
    }
    if (!functions->allocateMemory || !functions->freeMemory) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error", "fmi2Instantiate: Missing callback function.");
        return NULL;
    }
    if (!instanceName || strlen(instanceName) == 0) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error", "fmi2Instantiate: Missing instance name.");
        return NULL;
    }
    if (!fmuGUID || strlen(fmuGUID) == 0) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error", "fmi2Instantiate: Missing GUID.");
        return NULL;
    }

    /* allocate memory for Openmodelica Simulation Unit */
    OSU = functions->allocateMemory(1, sizeof(osu_t));
    if (!OSU) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error", "fmi2Instantiate: Not enough memory.");
        return NULL;
    }

    /* process XML file and read experiment_data and parts of model_data in osu_data*/
    OSU->osu_data = functions->allocateMemory(1, sizeof(omsi_t));
    omsi_char* initXMLFilename = functions->allocateMemory(20 + strlen(instanceName) + strlen(fmuResourceLocation), sizeof(omsi_char));
    sprintf(initXMLFilename, "%s/%s_init.xml", fmuResourceLocation, instanceName);
    if (omsu_process_input_xml(OSU->osu_data, initXMLFilename, fmuGUID, instanceName, functions)) {     // ToDo: needs some information beforehand
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error", "fmi2Instantiate: Could not process %s.", initXMLFilename);
        omsu_free_osu_data(OSU->osu_data, functions->freeMemory);
        return NULL;
    }
    functions->freeMemory(initXMLFilename);

    /* process JSON file and read missing parts of model_data in osu_data */
    omsi_char* infoJsonFilename = functions->allocateMemory(20 + strlen(instanceName) + strlen(fmuResourceLocation), sizeof(omsi_char));
    sprintf(infoJsonFilename, "%s/%s_info.json", fmuResourceLocation, instanceName);
    if (omsu_process_input_json(OSU->osu_data, infoJsonFilename, fmuGUID, instanceName, functions)) {     // ToDo: implement
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error", "fmi2Instantiate: Could not process %s.", infoJsonFilename);
        omsu_free_osu_data(OSU->osu_data, functions->freeMemory);
        return NULL;
    }
    functions->freeMemory(infoJsonFilename);

    /* allocate memory for sim_data */
    if (omsu_allocate_sim_data(OSU->osu_data, functions->allocateMemory)) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
            "fmi2Instantiate: Not enough memory.");
        return -1;
    }
    // ToDo: where do we get sim_data_t->inputs_real_index and so on? Probably from JSON file
    /* initialize sim_data */
//    omsi_model_setup_data(OSU);      // ToDo: implement, write stuff into sim_data


    OSU->osu_functions = (omsi_functions_t *) functions->allocateMemory(1, sizeof(omsi_functions_t));
    OSU->instanceName = (omsi_char*) functions->allocateMemory(1 + strlen(instanceName), sizeof(omsi_char));
    OSU->vrStates = (omsi_unsigned_int *) functions->allocateMemory(1, sizeof(omsi_unsigned_int));
    OSU->vrStatesDerivatives = (omsi_unsigned_int *) functions->allocateMemory(1, sizeof(omsi_unsigned_int));

    if (!OSU->osu_functions || !OSU->instanceName || !OSU->vrStates || !OSU->vrStatesDerivatives) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error", "fmi2Instantiate: Not enough memory.");
        return NULL;
    }


    // set all categories to on or off. fmi2SetDebugLogging should be called to choose specific categories.
    for (omsi_int i = 0; i < NUMBER_OF_CATEGORIES; i++) {
        OSU->logCategories[i] = loggingOn;
    }
    OSU->loggingOn = loggingOn;
    OSU->GUID = (omsi_char*) functions->allocateMemory(1, strlen(fmuGUID) + 1);
    if (!OSU->GUID) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error", "fmi2Instantiate: Not enough memory.");
        return NULL;
    }
    strcpy(OSU->GUID, fmuGUID);
    OSU->instanceName = strdup(instanceName);
    OSU->type = fmuType;
    OSU->fmiCallbackFunctions = functions;

    /* setup simulation data with default start values */
    //OSU->osu_functions->setupStartValues(OSU->osu_data);       // ToDo: implement. Probably pointer to generated function to set up default start data

    OSU->state = modelInstantiated;
    FILTERED_LOG(OSU, omsi_ok, LOG_FMI2_CALL, "fmi2Instantiate: GUID=%s", fmuGUID)
    omsu_print_debug(OSU);      // ToDo: delete
    return OSU;
}

/*
 * Informs the Openmodelica Simulation Unit to enter the initialization mode.
 */
omsi_status omsi_enter_initialization_mode(void* c) {
    osu_t* OSU = (osu_t *)c;

    if (invalidState(OSU, "fmi2EnterInitializationMode", modelInstantiated, ~0)) {
        FILTERED_LOG(OSU, omsi_error, LOG_FMI2_CALL, "fmi2EnterInitializationMode: call was not allowed", NULL)
        return omsi_error;
    }

    // ToDo: Do we need to do some stuff here? Maybee allocate memory for OSU->osu_data->sim_data->initialization and fill it.

    OSU->state = modelInitializationMode;
    FILTERED_LOG(OSU, omsi_ok, LOG_FMI2_CALL, "fmi2EnterInitializationMode: succeed", NULL)
    return omsi_ok;
}

/*
 * Informs the Openmodelica Simulation Unit to exit initialization mode.
 */
omsi_status omsi_exit_initialization_mode(void* c)
{
    osu_t* OSU = (osu_t *)c;
    if (invalidState(OSU, "fmi2ExitInitializationMode", modelInitializationMode, ~0))
        return omsi_error;
    FILTERED_LOG(OSU, omsi_ok, LOG_FMI2_CALL, "fmi2ExitInitializationMode...", NULL)

    // ToDo: free OSU->omsi_data->initialization
    // ToDo: allocate OSU->omsi_data->simulation here to save some memory?

    OSU->state = modelEventMode;
    FILTERED_LOG(OSU, omsi_ok, LOG_FMI2_CALL, "fmi2ExitInitializationMode: succeed", NULL)
        return omsi_ok;
}

/*
 * Setup experiment data for the Openmodelica Simulation Unit.
 * Gets called from function fmi2SetupExperiment.
 */
omsi_status omsi_setup_experiment(void*      c,
                                  omsi_bool  toleranceDefined,
                                  omsi_real  tolerance,
                                  omsi_real  startTime,
                                  omsi_bool  stopTimeDefined,
                                  omsi_real  stopTime) {
    osu_t* OSU = (osu_t *)c;

    if (invalidState(OSU, "fmi2SetupExperiment", modelInstantiated, ~0))
        return omsi_error;

    if (toleranceDefined) {
        OSU->osu_data->experiment->tolerance = tolerance;
    }
    else {
        OSU->osu_data->experiment->tolerance = 1e-5;    // default tolerance
    }
    OSU->osu_data->experiment->start_time = startTime;
    if (stopTimeDefined) {
        OSU->osu_data->experiment->stop_time = stopTime;
    }
    else {
        OSU->osu_data->experiment->stop_time = startTime+1;     // default stop time
    }
    OSU->osu_data->experiment->step_size = (OSU->osu_data->experiment->stop_time
                                            - OSU->osu_data->experiment->start_time) / 500;

    FILTERED_LOG(OSU, omsi_ok, LOG_FMI2_CALL,
        "fmi2SetupExperiment: toleranceDefined=%d tolerance=%g startTime=%g stopTimeDefined=%d stopTime=%g",
        toleranceDefined, OSU->osu_data->experiment->tolerance, startTime, stopTimeDefined, OSU->osu_data->experiment->stop_time)

    return omsi_ok;
}

/*
 * Frees all allocated memory for the Openmodelica Simulation Unit.
 * Does nothing if a null pointer is provided.
 * Gets called from function fmi2FreeInstance.
 */
void omsi_free_instance(void* c) {
    if (c==NULL) {
        return;
    }

    osu_t* OSU = (osu_t *)c;
    omsi_int meStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError;
    omsi_int csStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError;

    if (invalidState(OSU, "fmi2FreeInstance", meStates, csStates)) {
        return;
    }

    FILTERED_LOG(OSU, omsi_ok, LOG_FMI2_CALL, "fmi2FreeInstance", NULL)

    /* free OSU data */
    omsu_free_osu_data(OSU->osu_data, OSU->fmiCallbackFunctions->freeMemory);     // ToDo: implement, should free everything inside osu_data
    OSU->fmiCallbackFunctions->freeMemory(OSU->osu_data);
    // ToDo: free everything inside osu_functions
    OSU->fmiCallbackFunctions->freeMemory(OSU->osu_functions);

    /* free fmuData */
    OSU->fmiCallbackFunctions->freeMemory(OSU->threadData);

    OSU->fmiCallbackFunctions->freeMemory(OSU->old_data);
    /* free instanceName & GUID */
    if (OSU->instanceName) OSU->fmiCallbackFunctions->freeMemory((void*)OSU->instanceName);
    if (OSU->GUID) OSU->fmiCallbackFunctions->freeMemory((void*)OSU->GUID);

    /* free callback functions */
    OSU->fmiCallbackFunctions->freeMemory(OSU);
}

/*
 * Resets the Openmodelica Simulation Unit.
 * Gets called from function fmi2Reset.
 */
omsi_status omsi_reset(void* c) {
    osu_t* OSU = (osu_t *)c;
    if (invalidState(OSU, "fmi2Reset", modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0))
        return omsi_error;
    FILTERED_LOG(OSU, omsi_ok, LOG_FMI2_CALL, "fmi2Reset", NULL)

    if (OSU->state & modelTerminated) {
      /* initialize OSU */
//      omsic_model_setup_data(OSU);            // ToDo: implement
    }
    /* reset the values to start */
//    setDefaultStartValues(OSU);     // ToDo: implement

    OSU->state = modelInstantiated;
    return omsi_ok;
}

/*
 * Informs that the simulation run is terminated.
 * Gets called from function fmi2Terminate.
 */
omsi_status omsi_terminate(void* c) {
    osu_t* OSU = (osu_t *)c;
    if (invalidState(OSU, "fmi2Terminate", modelEventMode|modelContinuousTimeMode, ~0))
        return omsi_error;
    FILTERED_LOG(OSU, omsi_ok, LOG_FMI2_CALL, "fmi2Terminate", NULL)

    OSU->state = modelTerminated;
    return omsi_ok;
}
