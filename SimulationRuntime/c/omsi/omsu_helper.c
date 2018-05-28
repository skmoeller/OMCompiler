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
 * Allocates memory for omsi_t struct and all its components.
 * read_input_xml () must be called beforehand.
 */
int omsu_allocate_osu_data(omsi_t* omsi_data, const fmi2CallbackFunctions* functions, size_t lengthGUID) {

    int i,j;

    /* allocate memory for model data */
    omsi_data->model_data->model_vars_info_t = functions->allocateMemory(omsi_data->model_data->n_states, sizeof(model_variable_info_t));
    if (!omsi_data->model_data || !omsi_data->model_data->modelGUID || !omsi_data->model_data->model_vars_info_t) {
        return -1;
    }   // ToDo: add error cases for allocateMemory after every call. Should also return some error


    // allocate memory for model_variable_info_t
    for (i=0; i<omsi_data->model_data->n_real_vars; i++) {
        omsi_data->model_data->model_vars_info_t[i]->name = functions->allocateMemory(MAX_LENGTH_STRING, sizeof(char));
        omsi_data->model_data->model_vars_info_t[i]->comment = functions->allocateMemory(MAX_LENGTH_STRING, sizeof(char));

        // allocate memory for attribute
        real_var_attribute_t* attribute = functions->allocateMemory(1, sizeof(real_var_attribute_t));
        attribute->unit = functions->allocateMemory(MAX_LENGTH_STRING, sizeof(char));
        attribute->displayUnit = functions->allocateMemory(MAX_LENGTH_STRING, sizeof(char));
        omsi_data->model_data->model_vars_info_t[i]->attribute = attribute;
    }
    j = i;
    for (i=j; i<omsi_data->model_data->n_int_vars+j; i++) {
        omsi_data->model_data->model_vars_info_t[i]->name = functions->allocateMemory(MAX_LENGTH_STRING, sizeof(char));      // ToDo: find matching Macro
        omsi_data->model_data->model_vars_info_t[i]->comment = functions->allocateMemory(MAX_LENGTH_STRING, sizeof(char));
        omsi_data->model_data->model_vars_info_t[i]->attribute = (int_var_attribute_t) functions->allocateMemory(1, sizeof(int_var_attribute_t));
    }
    j = i;
    for (i=j; i<omsi_data->model_data->n_bool_vars+j; i++) {
        omsi_data->model_data->model_vars_info_t[i]->name = functions->allocateMemory(MAX_LENGTH_STRING, sizeof(char));      // ToDo: find matching Macro
        omsi_data->model_data->model_vars_info_t[i]->comment = functions->allocateMemory(MAX_LENGTH_STRING, sizeof(char));
        omsi_data->model_data->model_vars_info_t[i]->attribute = (bool_var_attribute_t) functions->allocateMemory(1, sizeof(bool_var_attribute_t));
    }
    j = i;
    for (i=j; i<omsi_data->model_data->n_string_vars+j; i++) {
        omsi_data->model_data->model_vars_info_t[i]->name = functions->allocateMemory(MAX_LENGTH_STRING, sizeof(char));      // ToDo: find matching Macro
        omsi_data->model_data->model_vars_info_t[i]->comment = functions->allocateMemory(MAX_LENGTH_STRING, sizeof(char));
        omsi_data->model_data->model_vars_info_t[i]->attribute = (string_var_attribute_t) functions->allocateMemory(1, sizeof(string_var_attribute_t));
    }

    // allocate memory for equation info
    omsi_data->model_data->equation_info_t = functions->allocateMemory(omsi_data->model_data->n_equations, sizeof(equation_info_t));
    for (i=0; i<omsi_data->model_data->n_equations; i++) {
        omsi_data->model_data->equation_info_t[i]->variables = functions->allocateMemory(omsi_data->model_data->equation_info_t[i]->numVar, sizeof(int));
    }

    // allocate memory for algebraic systems
    omsi_data->model_data->algebraic_system_t = functions->allocateMemory(omsi_data->model_data->n_algebraic_system, sizeof(omsi_algebraic_system_t));
    // ToDo: for loop to allocate inner stuff of algebraic systems

    /* allocate memory for simulation data */
    omsi_data->sim_data = functions->allocateMemory(1, sizeof(sim_data_t));
    omsi_data->sim_data->real_vars = functions->allocateMemory(omsi_data->model_data->n_real_vars + omsi_data->model_data->n_real_parameters, sizeof(real));
    omsi_data->sim_data->int_vars = functions->allocateMemory(omsi_data->model_data->n_int_vars + omsi_data->model_data->n_int_parameters, sizeof(int));
    omsi_data->sim_data->bool_vars = functions->allocateMemory(omsi_data->model_data->n_bool_vars + omsi_data->model_data->n_bool_parameters, sizeof(bool));

    omsi_data->sim_data->pre_real_vars = functions->allocateMemory(omsi_data->model_data->n_real_vars + omsi_data->model_data->n_real_parameters, sizeof(real));
    omsi_data->sim_data->pre_int_vars = functions->allocateMemory(omsi_data->model_data->n_int_vars + omsi_data->model_data->n_int_parameters, sizeof(int));
    omsi_data->sim_data->pre_bool_vars = functions->allocateMemory(omsi_data->model_data->n_bool_vars + omsi_data->model_data->n_bool_parameters, sizeof(bool));

    omsi_data->sim_data->zerocrossings_vars = functions->allocateMemory(omsi_data->model_data->n_zerocrossings, sizeof(bool));
    omsi_data->sim_data->pre_zerocrossings_vars = functions->allocateMemory(omsi_data->model_data->n_zerocrossings, sizeof(bool));

    /* allocate memory for experiment data */
    // already allocated in read_input_xml

    return 0;
}


/*
 * frees memory for omsi_t struct and all its components
 */
void omsu_free_osu_data(omsi_t* omsi_data,const fmi2CallbackFunctions* functions) {

    int i;

    /* free memory for model data */
    functions->freeMemory(omsi_data->model_data->modelGUID);

    for (i=0; i<omsi_data->model_data->n_real_vars; i++) {
        functions->freeMemory (omsi_data->model_data->model_vars_info_t[i]->name);
        functions->freeMemory (omsi_data->model_data->model_vars_info_t[i]->comment);
        real_var_attribute_t* attribute = omsi_data->model_data->model_vars_info_t[i]->attribute;
        free (attribute->unit);
        free (attribute->displayUnit);
        free (attribute);
        functions->freeMemory (omsi_data->model_data->model_vars_info_t[i]);    // ToDo: is this one really necessary? Or just  free(omsi_data->model_data->model_vars_info_t) at the end?
    }
    for (i=omsi_data->model_data->n_real_vars; i<omsi_data->model_data->n_states; i++) {
        functions->freeMemory (omsi_data->model_data->model_vars_info_t[i]->name);
        functions->freeMemory (omsi_data->model_data->model_vars_info_t[i]->comment);
        functions->freeMemory (omsi_data->model_data->model_vars_info_t[i]->attribute);
        functions->freeMemory (omsi_data->model_data->model_vars_info_t[i]);    // ToDo: is this one really necessary? Or just  free(omsi_data->model_data->model_vars_info_t) at the end?
    }
    functions->freeMemory (omsi_data->model_data->model_vars_info_t);

    for (i=0; i<omsi_data->model_data->n_equations; i++) {
        functions->freeMemory (omsi_data->model_data->equation_info_t[i]->variables);
    }
    functions->freeMemory (omsi_data->model_data->equation_info_t);

    for (i=0; i<omsi_data->model_data->n_algebraic_system; i++) {
        // functions->freeMemory (omsi_data->model_data->algebraic_system_t[i]->...);
    }
    functions->freeMemory (omsi_data->model_data->algebraic_system_t);

    functions->freeMemory (omsi_data->model_data);

    /* free memory for simulation data */
    functions->freeMemory (omsi_data->sim_data->real_vars);
    functions->freeMemory (omsi_data->sim_data->int_vars);
    functions->freeMemory (omsi_data->sim_data->bool_vars);
    functions->freeMemory (omsi_data->sim_data->pre_real_vars);
    functions->freeMemory (omsi_data->sim_data->pre_int_vars);
    functions->freeMemory (omsi_data->sim_data->pre_bool_vars);
    functions->freeMemory (omsi_data->sim_data->zerocrossings_vars);
    functions->freeMemory (omsi_data->sim_data->pre_zerocrossings_vars);
    functions->freeMemory (omsi_data->sim_data);

    /* free memory for experiment data */
    functions->freeMemory (omsi_data->experiment->solver_name);
    functions->freeMemory (omsi_data->experiment);

    /* free memory for omsi data */
    functions->freeMemory (omsi_data);

    return;
}


/*
 * Reads input values from a xml file.
 * Allocates some memory for osu_data. All other memory is allocated in function omsu_allocate_osu_data().
 */
void read_input_xml(omsi_t* osu_data, char* filename, fmi2String fmuGUID, fmi2String instanceName, const fmi2CallbackFunctions* functions) {

    /* Variables */
    int i,j;
    int done;
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
    }

    /* create the XML parser */
    parser = XML_ParserCreate(NULL);
    if(!parser) {
        fclose(file);
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
                                "fmi2Instantiate: Out of memory.");
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
        }
    } while(!done);
    fclose(file);

    // check model GUID
    guid = findHashStringStringNull(mi.md,"guid");
    if (NULL==guid) {
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
                "fmi2Instantiate: Model GUID %s is not set in model description %s.",
                fmuGUID, filename);
    }
    else if (strcmp(fmuGUID, guid)) {
        XML_ParserFree(parser);
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
            "fmi2Instantiate: Wrong GUID %s in file %s. Expected %s.",
            guid, filename, fmuGUID);
    }

    /* allocate necessary memory */
    osu_data = functions->allocateMemory(1, sizeof(omsi_t));
    osu_data->experiment = functions->allocateMemory(1, sizeof(omsi_experiment_t));
    osu_data->experiment->solver_name = functions->allocateMemory(MAX_LENGTH_STRING, sizeof(char));
    osu_data->model_data = functions->allocateMemory(1, sizeof(model_data_t));
    osu_data->model_data->modelGUID = functions->allocateMemory(fmuGUID + 1, sizeof(char));
    // ToDo: add error case

    /* read all experiment values */
    read_value_real(findHashStringString(mi.de,"startTime"), &(osu_data->experiment->start_time), 0);
    read_value_real(findHashStringString(mi.de,"stopTime"), &(osu_data->experiment->stop_time), osu_data->experiment->start_time+1);
    read_value_real(findHashStringString(mi.de,"stepSize"), &(osu_data->experiment->step_size), (osu_data->experiment->stop_time - osu_data->experiment->start_time) / 500);
    // ToDo: num_outputs missing in xml file?
    read_value_real(findHashStringString(mi.de,"tolerance"), &(osu_data->experiment->tolerance), 1e-5);
    read_value_string(findHashStringString(mi.de,"solver"), &(osu_data->experiment->solver_name));

    /* read all model values */
    read_value_string(findHashStringStringNull(mi.md,"guid"), &(osu_data->model_data->modelGUID));
    read_value_long(findHashStringString(mi.md,"numberOfContinuousStates"), &(osu_data->model_data->n_states), 0);
    read_value_long(findHashStringString(mi.md,"numberOfContinuousStates"), &(osu_data->model_data->n_derivatives), 0);
    read_value_long(findHashStringString(mi.md,"numberOfRealAlgebraicVariables"), &(osu_data->model_data->n_real_vars), 0);
    read_value_long(findHashStringString(mi.md,"numberOfIntegerAlgebraicVariables"), &(osu_data->model_data->n_int_vars), 0);
    read_value_long(findHashStringString(mi.md,"numberOfBooleanAlgebraicVariables"), &(osu_data->model_data->n_bool_vars), 0);
    read_value_long(findHashStringString(mi.md,"numberOfStringAlgebraicVariables"), &(osu_data->model_data->n_string_vars), 0);
    read_value_long(findHashStringString(mi.md,"numberOfRealParameters"), &(osu_data->model_data->n_real_parameters), 0);
    read_value_long(findHashStringString(mi.md,"numberOfIntegerParameters"), &(osu_data->model_data->n_int_parameters), 0);
    read_value_long(findHashStringString(mi.md,"numberOfBooleanParameters"), &(osu_data->model_data->n_bool_parameters), 0);
    read_value_long(findHashStringString(mi.md,"numberOfStringParameters"), &(osu_data->model_data->n_string_parameters), 0);
    // ToDo: read n_zerocrossings, model_vars_info_t, equation_info_t, algebraic_system_t, n_algebraic_system

    // ToDo: make stuff for alias variables, internal variables

    XML_ParserFree(parser);
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
