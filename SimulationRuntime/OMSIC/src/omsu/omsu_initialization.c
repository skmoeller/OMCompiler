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

#define DEBUG omsi_false
#define DEBUG_PRINT(function) if (DEBUG) {                                     \
    printf("\nDEBUG PRINT\n");                                                 \
    printf("=====================================================\n");         \
    fflush(stdout);                                                            \
    function; fflush(stdout);                                                  \
    }                                                                          \



#include <omsu_initialization.h>


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
    /* Variables */
    osu_t *OSU;
    omsi_char* initXMLFilename;
    omsi_char* infoJsonFilename;
    omsi_int i;



    /* set global callback functions */
    global_callback = (omsi_callback_functions*) functions;


    /* check all input arguments */
    /* ignoring arguments: fmuResourceLocation, visible */
    if (!functions->logger) {
        /* ToDo: Add error message, even if no logger is available */
        return NULL;
    }
    else {
        LOG_FILTER(NULL, LOG_ALL,
            functions->logger(NULL, instanceName, omsi_ok, logCategoriesNames[LOG_ALL], "Instantiate OSU."))
    }

    if (!functions->allocateMemory || !functions->freeMemory) {
        LOG_FILTER(NULL, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Missing callback function."))
        return NULL;
    }
    if (!instanceName || strlen(instanceName) == 0) {
        LOG_FILTER(NULL, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Missing instance name."))
        return NULL;
    }
    if (!fmuGUID || strlen(fmuGUID) == 0) {
        LOG_FILTER(NULL, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Missing GUID."))
        return NULL;
    }

    /* allocate memory for Openmodelica Simulation Unit */
    OSU = functions->allocateMemory(1, sizeof(osu_t));
    if (!OSU) {
        LOG_FILTER(OSU, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Not enough memory."))
        return NULL;
    }

    /* process XML file and read experiment_data and parts of model_data in osu_data*/
    OSU->osu_data = functions->allocateMemory(1, sizeof(omsi_t));
    /* ToDo Check error memory */
    initXMLFilename = functions->allocateMemory(20 + strlen(instanceName) + strlen(fmuResourceLocation), sizeof(omsi_char));
    sprintf(initXMLFilename, "%s/%s_init.xml", fmuResourceLocation, instanceName);
    if (omsu_process_input_xml(OSU->osu_data, initXMLFilename, fmuGUID, instanceName, functions)) {
        LOG_FILTER(OSU, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Could not process %s.", initXMLFilename))
        omsu_free_osu_data(OSU->osu_data);
        functions->freeMemory(OSU);
        return NULL;
    }
    functions->freeMemory(initXMLFilename);
    DEBUG_PRINT(omsu_print_model_data (OSU->osu_data->model_data, ""))

    /* process JSON file and read missing parts of model_data in osu_data */
    infoJsonFilename = functions->allocateMemory(20 + strlen(instanceName) + strlen(fmuResourceLocation), sizeof(omsi_char));
    sprintf(infoJsonFilename, "%s/%s_info.json", fmuResourceLocation, instanceName);
    if (omsu_process_input_json(OSU->osu_data, infoJsonFilename, fmuGUID, instanceName, functions)) {
        LOG_FILTER(OSU, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Could not process %s.", infoJsonFilename))
        omsu_free_osu_data(OSU->osu_data);
        functions->freeMemory(OSU);
        return NULL;
    }
    functions->freeMemory(infoJsonFilename);
    DEBUG_PRINT(omsu_print_omsi_t (OSU->osu_data, ""))

    /* Set template function pointers */
    OSU->osu_functions = (omsi_template_callback_functions_t *) functions->allocateMemory(1, sizeof(omsi_template_callback_functions_t));
    /* ToDo: actually set pointers */

    /* Instantiate and initialize sim_data */
    omsu_allocate_sim_data(OSU->osu_data, functions, instanceName);
    omsu_setup_sim_data(OSU->osu_data, OSU->osu_functions, OSU->fmiCallbackFunctions);
    DEBUG_PRINT(omsu_print_sim_data (OSU->osu_data->sim_data, ""))

    OSU->instanceName = (omsi_char*) functions->allocateMemory(1 + strlen(instanceName), sizeof(omsi_char));
    OSU->vrStates = (omsi_unsigned_int *) functions->allocateMemory(1, sizeof(omsi_unsigned_int));
    OSU->vrStatesDerivatives = (omsi_unsigned_int *) functions->allocateMemory(1, sizeof(omsi_unsigned_int));

    if (!OSU->osu_functions || !OSU->instanceName || !OSU->vrStates || !OSU->vrStatesDerivatives) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Not enough memory.");
        return NULL;
    }


    /* set all categories to on or off
     * fmi2SetDebugLogging should be called to choose specific categories. */
    for (i = 0; i < NUMBER_OF_CATEGORIES; i++) {
        OSU->logCategories[i] = loggingOn;
    }
    OSU->loggingOn = loggingOn;
    OSU->GUID = (omsi_char*) functions->allocateMemory(1, strlen(fmuGUID) + 1);
    if (!OSU->GUID) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Not enough memory.");
        return NULL;
    }
    strcpy(OSU->GUID, fmuGUID);
    OSU->instanceName = strdup(instanceName);
    OSU->type = fmuType;
    OSU->fmiCallbackFunctions = functions;

    /* setup simulation data with default start values */
    /*OSU->osu_functions->setupStartValues(OSU->osu_data);*/       /* ToDo: implement. Probably pointer to generated function to set up default start data */

    OSU->state = modelInstantiated;
    LOG_FILTER(OSU, LOG_ALL, functions->logger(OSU, global_instance_name, omsi_ok, logCategoriesNames[LOG_ALL], "fmi2Instantiate: GUID=%s", fmuGUID))

    DEBUG_PRINT(omsu_print_osu(OSU))
    return OSU;
}

/*
 * Informs the OpenModelica Simulation Unit to enter the initialization mode.
 */
omsi_status omsi_enter_initialization_mode(osu_t* OSU) {

    if (invalidState(OSU, "fmi2EnterInitializationMode", modelInstantiated, ~0)) {
        LOG_FILTER(OSU, LOG_STATUSERROR, global_callback->logger(OSU, global_instance_name, omsi_error, logCategoriesNames[LOG_STATUSERROR], "fmi2EnterInitializationMode: call was not allowed"))
        return omsi_error;
    }

    /* ToDo: Do we need to do some stuff here? Maybe allocate memory for OSU->osu_data->sim_data->initialization and fill it. */

    OSU->state = modelInitializationMode;
    LOG_FILTER(OSU, LOG_ALL, global_callback->logger(OSU, global_instance_name, omsi_ok, logCategoriesNames[LOG_ALL], "fmi2EnterInitializationMode: succeed"))
    return omsi_ok;
}

/*
 * Informs the OpenNodelica Simulation Unit to exit initialization mode.
 */
omsi_status omsi_exit_initialization_mode(osu_t* OSU) {

    if (invalidState(OSU, "fmi2ExitInitializationMode", modelInitializationMode, ~0))
        return omsi_error;
    LOG_FILTER(OSU, LOG_ALL, global_callback->logger(OSU, global_instance_name, omsi_ok, logCategoriesNames[LOG_ALL], "fmi2ExitInitializationMode..."))

    /* ToDo: free OSU->omsi_data->initialization */
    /* ToDo: allocate OSU->omsi_data->simulation here to save some memory? */

    OSU->state = modelEventMode;
    LOG_FILTER(OSU, LOG_ALL,
        global_callback->logger(OSU, global_instance_name, omsi_ok, logCategoriesNames[LOG_ALL], "fmi2ExitInitializationMode: succeed"))

    return omsi_ok;
}

/*
 * Setup experiment data for the Openmodelica Simulation Unit.
 * Gets called from function fmi2SetupExperiment.
 */
omsi_status omsi_setup_experiment(osu_t*     OSU,
                                  omsi_bool  toleranceDefined,
                                  omsi_real  tolerance,
                                  omsi_real  startTime,
                                  omsi_bool  stopTimeDefined,
                                  omsi_real  stopTime) {

    if (invalidState(OSU, "fmi2SetupExperiment", modelInstantiated, ~0))
        return omsi_error;

    if (toleranceDefined) {
        OSU->osu_data->experiment->tolerance = tolerance;
    }
    else {
        OSU->osu_data->experiment->tolerance = 1e-5;    /* default tolerance */
    }
    OSU->osu_data->experiment->start_time = startTime;
    if (stopTimeDefined) {
        OSU->osu_data->experiment->stop_time = stopTime;
    }
    else {
        OSU->osu_data->experiment->stop_time = startTime+1;     /* default stop time */
    }
    OSU->osu_data->experiment->step_size = (OSU->osu_data->experiment->stop_time
                                            - OSU->osu_data->experiment->start_time) / 500;

    LOG_FILTER(OSU, LOG_ALL,
            global_callback->logger(OSU, global_instance_name, omsi_ok, logCategoriesNames[LOG_ALL], "fmi2SetupExperiment: toleranceDefined=%d tolerance=%g startTime=%g stopTimeDefined=%d stopTime=%g",
                    toleranceDefined, OSU->osu_data->experiment->tolerance, startTime, stopTimeDefined, OSU->osu_data->experiment->stop_time))

    return omsi_ok;
}

/*
 * Frees all allocated memory for the Openmodelica Simulation Unit.
 * Does nothing if a null pointer is provided.
 * Gets called from function fmi2FreeInstance.
 */
void omsi_free_instance(osu_t* OSU) {

    /* Variables */
    omsi_int meStates;
    omsi_int csStates;

    if (OSU==NULL) {
        return;
    }

    meStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError;
    csStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError;

    if (invalidState(OSU, "fmi2FreeInstance", meStates, csStates)) {
        return;
    }

    LOG_FILTER(OSU, LOG_ALL,
                global_callback->logger(OSU, global_instance_name, omsi_ok, logCategoriesNames[LOG_ALL], "fmi2FreeInstance"))


    /* free OSU data */
    omsu_free_osu_data(OSU->osu_data);
    OSU->fmiCallbackFunctions->freeMemory(OSU->osu_data);
    /* ToDo: free everything inside osu_functions */
    OSU->fmiCallbackFunctions->freeMemory(OSU->osu_functions);

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
omsi_status omsi_reset(osu_t* OSU) {

    if (invalidState(OSU, "fmi2Reset", modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0))
        return omsi_error;

    LOG_FILTER(OSU, LOG_ALL,
                    global_callback->logger(OSU, global_instance_name, omsi_ok, logCategoriesNames[LOG_ALL], "fmi2Reset"))

    if (OSU->state & modelTerminated) {
      /* initialize OSU */
      /* omsic_model_setup_data(OSU);*/            /* ToDo: implement */
    }
    /* reset the values to start */
    /*setDefaultStartValues(OSU);*/     /* ToDo: implement */

    OSU->state = modelInstantiated;
    return omsi_ok;
}

/*
 * Informs that the simulation run is terminated.
 * Gets called from function fmi2Terminate.
 */
omsi_status omsi_terminate(osu_t* OSU) {

    if (invalidState(OSU, "fmi2Terminate", modelEventMode|modelContinuousTimeMode, ~0))
        return omsi_error;

    LOG_FILTER(OSU, LOG_ALL,
                        global_callback->logger(OSU, global_instance_name, omsi_ok, logCategoriesNames[LOG_ALL], "fmi2Terminate"))

    OSU->state = modelTerminated;
    return omsi_ok;
}
