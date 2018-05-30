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

#include "omsu_helper.h"
#include "../simulation/simulation_input_xml.c"     // ToDo: Change! Needing structs, but those are not in .h defined

#define MAX_LENGTH_STRING 42      //ToDo: find matching Macro or set to something smarter


void storePreValues (DATA *data) {
    //TODO: implement for new data structure
}

int stateSelection(DATA *data, threadData_t *threadData, char reportError, int switchStates) {
    //TODO: implement for new data structure
}

void overwriteOldSimulationData(DATA *data) {
    //TODO: implement for new data structure
}





/*
 * frees memory for omsi_t struct and all its components
 */
void omsu_free_osu_data(omsi_t* omsi_data,const fmi2CallbackFreeMemory freeMemory) {

    int i;
    int n_vars_and_params;

    /* free memory for model data */
    freeMemory(omsi_data->model_data->modelGUID);

    for (i=0; i<omsi_data->model_data->n_real_vars; i++) {
        freeMemory (omsi_data->model_data->model_vars_info_t[i]->name);
        freeMemory (omsi_data->model_data->model_vars_info_t[i]->comment);
        real_var_attribute_t* attribute = omsi_data->model_data->model_vars_info_t[i]->attribute;
        free (attribute->unit);
        free (attribute->displayUnit);
        free (attribute);
        freeMemory (omsi_data->model_data->model_vars_info_t[i]);    // ToDo: is this one really necessary? Or just  free(omsi_data->model_data->model_vars_info_t) at the end?
    }
    n_vars_and_params = omsi_data->model_data->n_real_vars + omsi_data->model_data->n_int_vars
                       + omsi_data->model_data->n_bool_vars + omsi_data->model_data->n_string_vars;
    for (i=omsi_data->model_data->n_real_vars; i<n_vars_and_params; i++) {
        freeMemory (omsi_data->model_data->model_vars_info_t[i]->name);
        freeMemory (omsi_data->model_data->model_vars_info_t[i]->comment);
        freeMemory (omsi_data->model_data->model_vars_info_t[i]->attribute);
        freeMemory (omsi_data->model_data->model_vars_info_t[i]);    // ToDo: is this one really necessary? Or just  free(omsi_data->model_data->model_vars_info_t) at the end?
    }
    freeMemory (omsi_data->model_data->model_vars_info_t);

    for (i=0; i<omsi_data->model_data->n_equations; i++) {
        freeMemory (omsi_data->model_data->equation_info_t[i]->variables);
    }
    freeMemory (omsi_data->model_data->equation_info_t);

    freeMemory (omsi_data->model_data);

    /* free memory for simulation data */
    // ToDo: free inner stuff of initialization
    freeMemory (omsi_data->sim_data->initialization);
    // ToDo: free inner stuff of initialization
    freeMemory (omsi_data->sim_data->simulation);


    freeMemory (omsi_data->sim_data->model_vars_and_params->reals);
    freeMemory (omsi_data->sim_data->model_vars_and_params->ints);
    freeMemory (omsi_data->sim_data->model_vars_and_params->bools);

    freeMemory (omsi_data->sim_data->zerocrossings_vars);
    freeMemory (omsi_data->sim_data->pre_zerocrossings_vars);
    freeMemory (omsi_data->sim_data);

    /* free memory for experiment data */
    freeMemory (omsi_data->experiment->solver_name);
    freeMemory (omsi_data->experiment);

    /* free memory of omsi_data */
    freeMemory (omsi_data);

    return;
}

/* reads modelica_string value from a string */
static inline void omsu_read_value_string(const char *s, const char **str) {
    if(str == NULL) {
        warningStreamPrint(LOG_SIMULATION, 0, "error read_value, no data allocated for storing string");    // ToDo: Use same log everywhere
        return;
    }
    *str = strdup(s);
    if (str == NULL) {
        warningStreamPrint(LOG_SIMULATION, 0, "error read_value, out of memory");       // ToDo: Use same log everywhere
        return;
    }
}


/*
 * Reads input values from a xml file and allocates memory for osu_data struct.
 */
int omsu_process_input_xml(omsi_t* osu_data, char* filename, fmi2String fmuGUID, fmi2String instanceName, const fmi2CallbackFunctions* functions) {

    /* Variables */
    int i,j;
    int done;
    int n_model_vars_and_params;
    const char *guid;
    char buf[BUFSIZ] = {0};

    omc_ModelInput mi = {0};
    FILE* file = NULL;
    XML_Parser parser = NULL;

    /* open xml file */
    file = fopen(filename, "r");
    if(!file) {
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
                        "fmi2Instantiate: Can not read model description file %s.", filename);
        return -1;
    }

    /* create the XML parser */
    parser = XML_ParserCreate("UTF-8");
    if(!parser) {
        fclose(file);
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
                                "fmi2Instantiate: Out of memory.");
        return -1;
    }
    /* set our user data */
    XML_SetUserData(parser, &mi);
    /* set the handlers for start/end of element. */
    XML_SetElementHandler(parser, startElement, endElement);

    /* read XML */
    do {
        size_t len = fread(buf, 1, sizeof(buf), file);
        done = len < sizeof(buf);
        if(XML_STATUS_ERROR == XML_Parse(parser, buf, len, done)) {
            fclose(file);
            functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
                "fmi2Instantiate: failed to read the XML file %s: %s at line %lu.", filename,
                XML_ErrorString(XML_GetErrorCode(parser)),
                XML_GetCurrentLineNumber(parser));
            XML_ParserFree(parser);
            return -1;
        }
    } while(!done);

    fclose(file);
    XML_ParserFree(parser);

    // check model GUID
    guid = findHashStringStringNull(mi.md,"guid");
    if (NULL==guid) {
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
            "fmi2Instantiate: Model GUID %s is not set in model description %s.",
            fmuGUID, filename);
        return -1;
    }
    else if (strcmp(fmuGUID, guid)) {
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
            "fmi2Instantiate: Wrong GUID %s in file %s. Expected %s.",
            guid, filename, fmuGUID);
        return -1;
    }


    /* process experiment data */
    osu_data->experiment = functions->allocateMemory(1, sizeof(omsi_experiment_t));
    if (!osu_data->experiment) {
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
            "fmi2Instantiate: Not enough memory.");
        return -1;
    }

    read_value_real(findHashStringString(mi.de,"startTime"), &(osu_data->experiment->start_time), 0);
    read_value_real(findHashStringString(mi.de,"stopTime"), &(osu_data->experiment->stop_time), osu_data->experiment->start_time+1);
    read_value_real(findHashStringString(mi.de,"stepSize"), &(osu_data->experiment->step_size), (osu_data->experiment->stop_time - osu_data->experiment->start_time) / 500);
    // ToDo: find num_outputs
    read_value_real(findHashStringString(mi.de,"tolerance"), &(osu_data->experiment->tolerance), 1e-5);
    omsu_read_value_string(findHashStringString(mi.de,"solver"), &(osu_data->experiment->solver_name));


    /* process all model data */
    osu_data->model_data = functions->allocateMemory(1, sizeof(model_data_t));

    omsu_read_value_string(findHashStringStringNull(mi.md,"guid"), &(osu_data->model_data->modelGUID));
    read_value_int(findHashStringString(mi.md,"numberOfContinuousStates"), &(osu_data->model_data->n_states), 0);
    read_value_int(findHashStringString(mi.md,"numberOfContinuousStates"), &(osu_data->model_data->n_derivatives), 0);
    read_value_int(findHashStringString(mi.md,"numberOfRealAlgebraicVariables"), &(osu_data->model_data->n_real_vars), 0);
    read_value_int(findHashStringString(mi.md,"numberOfIntegerAlgebraicVariables"), &(osu_data->model_data->n_int_vars), 0);
    read_value_int(findHashStringString(mi.md,"numberOfBooleanAlgebraicVariables"), &(osu_data->model_data->n_bool_vars), 0);
    read_value_int(findHashStringString(mi.md,"numberOfStringAlgebraicVariables"), &(osu_data->model_data->n_string_vars), 0);
    read_value_int(findHashStringString(mi.md,"numberOfRealParameters"), &(osu_data->model_data->n_real_parameters), 0);
    read_value_int(findHashStringString(mi.md,"numberOfIntegerParameters"), &(osu_data->model_data->n_int_parameters), 0);
    read_value_int(findHashStringString(mi.md,"numberOfBooleanParameters"), &(osu_data->model_data->n_bool_parameters), 0);
    read_value_int(findHashStringString(mi.md,"numberOfStringParameters"), &(osu_data->model_data->n_string_parameters), 0);
    // ToDo: read n_zerocrossings, model_vars_info_t, equation_info_t, algebraic_system_t, n_algebraic_system

    read_value_int(findHashStringString(mi.md,"numberOfEquations"), &(osu_data->model_data->n_equations), 0);      // ToDo: Is numberOfEquations in XML???
    n_model_vars_and_params = osu_data->model_data->n_real_vars + osu_data->model_data->n_int_vars
                            + osu_data->model_data->n_bool_vars + osu_data->model_data->n_string_vars
                            + osu_data->model_data->n_real_parameters + osu_data->model_data->n_int_parameters
                            + osu_data->model_data->n_bool_parameters + osu_data->model_data->n_string_parameters;
    osu_data->model_data->model_vars_info_t = (model_variable_info_t*) functions->allocateMemory(n_model_vars_and_params, sizeof(model_variable_info_t));
    if (!osu_data->model_data->model_vars_info_t) {
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
            "fmi2Instantiate: Not enough memory.");
        return -1;
    }

    osu_data->model_data->equation_info_t = (equation_info_t*) functions->allocateMemory(osu_data->model_data->n_equations, sizeof(equation_info_t));

    /* now all data from init_xml should be utilized */
    // ToDo: free mi

    // information for model_vars_info_t and equation_info_t are in JSON file
    // ToDo: read JSON file somewhere

    /* allocate memory for sim_data_t */
    if (!omsu_allocate_sim_data(osu_data, functions)) {
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
            "fmi2Instantiate: Not enough memory.");
        return -1;
    }
    // ToDo: where do we get sim_data_t->inputs_real_index and so on?

   // ToDo: add error case



    return 0;
}

/*
 * Allocates memory for sim_data_t struct and all its components.
 * Gets called from omsu_process_input_xml().
 */
int omsu_allocate_sim_data(omsi_t* omsi_data, const fmi2CallbackAllocateMemory allocateMemory) {

    /* allocate memory for simulation data */
    omsi_data->sim_data = (sim_data_t*) allocateMemory(1, sizeof(sim_data_t));

    omsi_data->sim_data->initialization = (equation_system_t*) allocateMemory(1, sizeof(equation_system_t));
    // ToDo: add stuff
    omsi_data->sim_data->simulation = (equation_system_t*) allocateMemory(1, sizeof(equation_system_t));
    // ToDo: add more stuff

    omsi_data->sim_data->model_vars_and_params = (omsi_values *) allocateMemory(someNumber, sizeof(omsi_values));
    omsi_data->sim_data->model_vars_and_params->reals = (double *) allocateMemory(omsi_data->model_data->n_real_vars + omsi_data->model_data->n_real_parameters, sizeof(double));
    omsi_data->sim_data->model_vars_and_params->ints = (int*) allocateMemory(omsi_data->model_data->n_int_vars + omsi_data->model_data->n_int_parameters, sizeof(int));
    omsi_data->sim_data->model_vars_and_params->bools = (bool*) allocateMemory(omsi_data->model_data->n_bool_vars + omsi_data->model_data->n_bool_parameters, sizeof(bool));

    // ToDo: allocate memory for some pre values

    omsi_data->sim_data->zerocrossings_vars = (bool *) allocateMemory(omsi_data->model_data->n_zerocrossings, sizeof(bool));
    omsi_data->sim_data->pre_zerocrossings_vars = (bool *) allocateMemory(omsi_data->model_data->n_zerocrossings, sizeof(bool));

    return 0;
}

int initializeNonlinearSystems(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
}

int initializeLinearSystems(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
}

int initializeMixedSystems(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
}

void initializeStateSetJacobians(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
}

void setZCtol(double relativeTol) {
    //ToDO: implement
}

void copyStartValuestoInitValues(DATA *data) {
    //ToDo: implement for new data structure
}

int initialization(DATA *data, threadData_t *threadData, const char* pInitMethod, const char* pInitFile, double initTime) {
    //ToDo: implement for new data structure
}

void initSample(DATA* data, threadData_t *threadData, double startTime, double stopTime) {
    //ToDo: implement for new data structure
}

void initDelay(DATA* data, double startTime) {
    //ToDo: implement for new data structure
}

double getNextSampleTimeFMU(DATA *data) {
    //ToDo: implement for new data structure
}

void setAllVarsToStart(DATA *data) {
    //ToDo: implement for new data structure
}

void setAllParamsToStart(DATA *data) {
    //ToDo: implement for new data structure
}

int freeNonlinearSystems(DATA *data, threadData_t *threadData) {
    //ToDo: implement for new data structure
}

int freeMixedSystems(DATA *data, threadData_t *threadData) {
    //ToDo: implement for new data structure
}

int freeLinearSystems(DATA *data, threadData_t *threadData) {
    //ToDo: implement for new data structure
}

void freeStateSetData(DATA *data) {
    //ToDo: implement for new data structure
}

void deInitializeDataStruc(DATA *data) {
    //ToDo: implement for new data structure
}

void updateRelationsPre(DATA *data) {
    //ToDo: implement for new data structure
}

modelica_boolean checkRelations(DATA *data) {
    //ToDo: implement for new data structure
}

void mmc_catch_dummy_fn (void) {
    //ToDo: delete
}

void omsic_model_setup_data (osu_t* OSU) {
    //ToDo: delete
}
