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

#include <omsi_initialization.h>

/*
 * Allocates memory for the Openmodelica Simulation Unit and initializes it.
 */
omsi_t* omsi_instantiate(omsi_string                            instanceName,
                         omsu_type                              fmuType,
                         omsi_string                            fmuGUID,
                         omsi_string                            fmuResourceLocation,
                         const omsi_callback_functions*         functions,
                         omsi_template_callback_functions_t*    template_functions,
                         omsi_bool                              __attribute__((unused)) visible,
                         omsi_bool                              loggingOn)
{
    /* Variables */
    omsi_t* osu_data;
    omsi_char* initXMLFilename;
    omsi_char* infoJsonFilename;
    omsi_status status;


    /* set global callback functions */
    global_callback = (omsi_callback_functions*) functions;
    global_instance_name = instanceName;


    /* check all input arguments */
    /* ignoring arguments: fmuResourceLocation, visible */
    if (!functions->logger) {
        /* ToDo: Add error message, even if no logger is available */
        return NULL;
    }

    /* Log function call */
    LOG_FILTER(NULL, LOG_FMI2_CALL,
        functions->logger(NULL, instanceName, omsi_ok, logCategoriesNames[LOG_FMI2_CALL],
        "fmi2Instantiate: Instantiate OSU."))

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


    /* process XML file and read experiment_data and parts of model_data in osu_data*/
    osu_data = functions->allocateMemory(1, sizeof(omsi_t));
    /* ToDo Check error memory */
    initXMLFilename = functions->allocateMemory(20 + strlen(instanceName) + strlen(fmuResourceLocation), sizeof(omsi_char));
    sprintf(initXMLFilename, "%s\%s_init.xml", fmuResourceLocation, instanceName);
    if (omsu_process_input_xml(osu_data, initXMLFilename, fmuGUID, instanceName, functions) == omsi_error) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Could not process %s.", initXMLFilename))
        omsu_free_osu_data(osu_data);
        return NULL;
    }
    functions->freeMemory(initXMLFilename);

    /* process JSON file and read missing parts of model_data in osu_data */
    infoJsonFilename = functions->allocateMemory(20 + strlen(instanceName) + strlen(fmuResourceLocation), sizeof(omsi_char));
    sprintf(infoJsonFilename, "%s/%s_info.json", fmuResourceLocation, instanceName);
    if (omsu_process_input_json(osu_data, infoJsonFilename, fmuGUID, instanceName, functions) == omsi_error) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR],
            "fmi2Instantiate: Could not process %s.", infoJsonFilename))
        omsu_free_osu_data(osu_data);
        return NULL;
    }
    functions->freeMemory(infoJsonFilename);

    /* Instantiate and initialize sim_data */
    status = omsu_allocate_sim_data(osu_data, functions, instanceName);
    if (status != omsi_ok) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error,
            logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Could not allocate memory for sim_data."))
         return NULL;
    }

    status = omsi_allocate_model_variables(osu_data, functions);   /* ToDo: move this function into omsu_allocate_sim_data */
    if (status != omsi_ok) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error,
            logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Could not allocate memory for sim_data->model_vars_and_params."))
         return NULL;
    }

    /*status = omsu_setup_sim_data(osu_data, OSU->osu_functions, functions);*/
    status = omsu_setup_sim_data(osu_data, template_functions, functions);
    if (status != omsi_ok) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error,
            logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Could not initialize sim_data->simulation."))
         return NULL;
    }

    status = omsi_initialize_model_variables(osu_data, functions, instanceName);
    if (status != omsi_ok) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error,
            logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Could not initialize sim_data->model_vars_and_params."))
         return NULL;
    }

    return osu_data;
}
