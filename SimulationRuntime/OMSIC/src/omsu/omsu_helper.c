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

#include "omsu/omsu_helper.h"

#define UNUSED(x) (void)(x)     // ToDo: delete later

/* forward global functions */
omsi_callback_allocate_memory   global_allocateMemory;
omsi_callback_free_memory       global_freeMemory;

void storePreValues (DATA *data) {
    UNUSED(data);
    //TODO: implement for new data structure
}

omsi_int stateSelection(DATA *data, threadData_t *threadData, omsi_char reportError, omsi_int switchStates) {
    //TODO: implement for new data structure
    UNUSED(data); UNUSED(threadData); UNUSED(reportError); UNUSED(switchStates);
    return -1;
}

void overwriteOldSimulationData(DATA *data) {
    UNUSED(data);
    //TODO: implement for new data structure
}



/*
 * frees memory for omsi_t struct and all its components
 */
void omsu_free_osu_data(omsi_t* omsi_data, const omsi_callback_free_memory freeMemory) {

    omsi_unsigned_int i, j=0;
    omsi_unsigned_int n_model_vars_and_params;

    /* free memory for model data */
    freeMemory((omsi_string)omsi_data->model_data.modelGUID);

    for (i=0; i<omsi_data->model_data.n_real_vars+omsi_data->model_data.n_real_parameters; i++, j++) {
        freeMemory (omsi_data->model_data.model_vars_info_t[j].name);
        freeMemory (omsi_data->model_data.model_vars_info_t[j].comment);
        real_var_attribute_t* attribute = omsi_data->model_data.model_vars_info_t[j].modelica_attributes;
        freeMemory (attribute->unit);
        freeMemory (attribute->displayUnit);
        freeMemory (omsi_data->model_data.model_vars_info_t[j].modelica_attributes);
        freeMemory(omsi_data->model_data.model_vars_info_t[j].info.filename);
    }
    n_model_vars_and_params = omsi_data->model_data.n_states + omsi_data->model_data.n_derivatives
                             + omsi_data->model_data.n_real_vars + omsi_data->model_data.n_int_vars
                             + omsi_data->model_data.n_bool_vars + omsi_data->model_data.n_string_vars
                             + omsi_data->model_data.n_real_parameters + omsi_data->model_data.n_int_parameters
                             + omsi_data->model_data.n_bool_parameters + omsi_data->model_data.n_string_parameters
                             + omsi_data->model_data.n_real_aliases + omsi_data->model_data.n_int_aliases
                             + omsi_data->model_data.n_bool_aliases + omsi_data->model_data.n_string_aliases;
    for (i=j; i<n_model_vars_and_params; i++, j++) {
        freeMemory (omsi_data->model_data.model_vars_info_t[j].name);
        freeMemory (omsi_data->model_data.model_vars_info_t[j].comment);
        freeMemory (omsi_data->model_data.model_vars_info_t[j].modelica_attributes);
        freeMemory(omsi_data->model_data.model_vars_info_t[j].info.filename);
    }
    freeMemory (omsi_data->model_data.model_vars_info_t);

//    for (i=0; i<omsi_data->model_data.n_equations; i++) {
//        freeMemory (omsi_data->model_data.equation_info_t[i].variables);
//    }
    freeMemory (omsi_data->model_data.equation_info_t);

    /* free memory for simulation data */
    // ToDo: free inner stuff of initialization
    freeMemory (omsi_data->sim_data.initialization);
    // ToDo: free inner stuff of initialization
    freeMemory (omsi_data->sim_data.simulation);

    freeMemory (omsi_data->sim_data.model_vars_and_params->reals);
    freeMemory (omsi_data->sim_data.model_vars_and_params->ints);
    freeMemory (omsi_data->sim_data.model_vars_and_params->bools);

    freeMemory (omsi_data->sim_data.zerocrossings_vars);
    freeMemory (omsi_data->sim_data.pre_zerocrossings_vars);

    /* free memory for experiment data */
    freeMemory ((omsi_char *)omsi_data->experiment->solver_name);        // type-cast to shut of warning when compiling
    freeMemory (omsi_data->experiment);
}





/*
 * Allocates memory for sim_data_t struct and all its components.
 * Gets called from omsu_process_input_xml().
 */
omsi_int omsu_allocate_sim_data(omsi_t* omsi_data, const omsi_callback_allocate_memory allocateMemory) {

    omsi_int n_model_vars_and_params;

    omsi_data->sim_data.initialization = (omsi_function_t*) allocateMemory(1, sizeof(omsi_function_t));
    if (!omsi_data->sim_data.initialization) {
        return -1;      // Error: Out of memory
    }
    // ToDo: add stuff, e.g. allocate memory for all parts of struct initialization

    omsi_data->sim_data.simulation = (omsi_function_t*) allocateMemory(1, sizeof(omsi_function_t));
    if (!omsi_data->sim_data.initialization) {
        return -1;      // Error: Out of memory
    }
    // ToDo: add more stuff

    n_model_vars_and_params = omsi_data->model_data.n_real_vars + omsi_data->model_data.n_int_vars + omsi_data->model_data.n_bool_vars + omsi_data->model_data.n_string_vars
                              + omsi_data->model_data.n_real_parameters + omsi_data->model_data.n_int_parameters + omsi_data->model_data.n_bool_parameters + omsi_data->model_data.n_string_parameters;
    omsi_data->sim_data.model_vars_and_params = (omsi_values *) allocateMemory(n_model_vars_and_params, sizeof(omsi_values));
    if (!omsi_data->sim_data.model_vars_and_params) {
        return -1;      // Error: Out of memory
    }
    omsi_data->sim_data.model_vars_and_params->reals = (omsi_real *) allocateMemory(omsi_data->model_data.n_real_vars + omsi_data->model_data.n_real_parameters, sizeof(omsi_real));
    omsi_data->sim_data.model_vars_and_params->ints = (omsi_int*) allocateMemory(omsi_data->model_data.n_int_vars + omsi_data->model_data.n_int_parameters, sizeof(omsi_int));
    omsi_data->sim_data.model_vars_and_params->bools = (omsi_bool*) allocateMemory(omsi_data->model_data.n_bool_vars + omsi_data->model_data.n_bool_parameters, sizeof(omsi_bool));
    if (!omsi_data->sim_data.model_vars_and_params->reals || !omsi_data->sim_data.model_vars_and_params->ints || !omsi_data->sim_data.model_vars_and_params->bools ) {
        return -1;      // Error: Out of memory
    }

    // ToDo: allocate memory for some pre-values

    omsi_data->sim_data.zerocrossings_vars = (omsi_bool *) allocateMemory(omsi_data->model_data.n_zerocrossings, sizeof(omsi_bool));
    omsi_data->sim_data.pre_zerocrossings_vars = (omsi_bool *) allocateMemory(omsi_data->model_data.n_zerocrossings, sizeof(omsi_bool));

    // ToDo: Add error cases

    return 0;
}

void omsu_print_debug_helper (model_data_t* model_data, omsi_unsigned_int start_index, omsi_unsigned_int size, omsi_unsigned_int indent) {

    omsi_char* oneIndent = "| ";
    omsi_char* allIndent = (omsi_char*) global_allocateMemory(100, sizeof(omsi_char));


    omsi_unsigned_int i;
    for (i=0; i<indent; i++) {
        strcat(allIndent, oneIndent);
    }

    real_var_attribute_t* attribute_real;
    int_var_attribute_t* attribute_integer;
    bool_var_attribute_t* attribute_bool;
    string_var_attribute_t* attribute_string;
    for (i=0; i<size; i++) {
        printf("%sid:\t\t\t%i\n", allIndent, model_data->model_vars_info_t[start_index+i].id);
        printf("%sname:\t\t\t%s\n", allIndent, model_data->model_vars_info_t[start_index+i].name);
        printf("%scomment:\t\t\t%s\n", allIndent, model_data->model_vars_info_t[start_index+i].comment);
        printf("%svariable type:\t\t%i\n", allIndent, (omsi_int)model_data->model_vars_info_t[start_index+i].type_index.type);
        printf("%svariable index:\t\t%i\n", allIndent, model_data->model_vars_info_t[start_index+i].type_index.index);
        printf("%sattribute:\n", allIndent);

        switch (model_data->model_vars_info_t[start_index+i].type_index.type) {
        case OMSI_TYPE_REAL:
            attribute_real = model_data->model_vars_info_t[start_index+i].modelica_attributes;
            printf("| %sunit:\t\t\t%s\n", allIndent, attribute_real->unit);
            printf("| %sdisplayUnit:\t\t%s\n", allIndent, attribute_real->displayUnit);
            if (attribute_real->min <= -OMSI_DBL_MAX) {
                printf("| %smin:\t\t\t-infinity\n", allIndent);
            }
            else {
                printf("| %smin:\t\t\t%f\n", allIndent, attribute_real->min);
            }
            if (attribute_real->max >= OMSI_DBL_MAX) {
                printf("| %smax:\t\t\tininity\n", allIndent);
            }
            else {
                printf("| %smax:\t\t\t%f\n", allIndent, attribute_real->max);
            }
            printf("| %sfixed:\t\t\t%s\n", allIndent, attribute_real->fixed ? "true" : "false");
            printf("| %snominal:\t\t%f\n", allIndent, attribute_real->nominal);
            printf("| %sstart:\t\t\t%f\n", allIndent, attribute_real->start);
            break;
        case OMSI_TYPE_INTEGER:
            attribute_integer = model_data->model_vars_info_t[start_index+i].modelica_attributes;
            if (attribute_integer->min <= -OMSI_INT_MAX) {
                printf("| %smin:\t\t\t-infinity\n", allIndent);
            }
            else {
                printf("| %smin:\t\t\t%i\n", allIndent, attribute_integer->min);
            }
            if (attribute_integer->max >= OMSI_INT_MAX) {
                printf("| %smax:\t\t\tininity\n", allIndent);
            }
            else {
                printf("| %smax:\t\t\t%i\n", allIndent, attribute_integer->max);
            }
            printf("| %sfixed:\t\t\t%s\n", allIndent, attribute_integer->fixed ? "true" : "false");
            printf("| %sstart:\t\t\t%i\n", allIndent, attribute_integer->start);
            break;
        case OMSI_TYPE_BOOLEAN:
            attribute_bool = model_data->model_vars_info_t[start_index+i].modelica_attributes;
            printf("| %sfixed:\t\t\t%s\n", allIndent, attribute_bool->fixed ? "true" : "false");
            printf("| %sstart:\t\t\t%s\n", allIndent, attribute_bool->start ? "true" : "false");
            break;
        case OMSI_TYPE_STRING:
            attribute_string = model_data->model_vars_info_t[start_index+i].modelica_attributes;
            printf("| %sstart:\t\t\t%s\n", allIndent, attribute_string->start);
            break;
        default:
            // ToDo: plot warning
            break;
        }

        printf("%salias:\t\t\t%s\n",  allIndent, model_data->model_vars_info_t[start_index+i].isAlias ? "true" : "false");
        printf("%snegate:\t\t\t%i\n",  allIndent, model_data->model_vars_info_t[start_index+i].negate);
        printf("%saliasID:\t\t\t%i\n",  allIndent, model_data->model_vars_info_t[start_index+i].aliasID);

        printf("%sfile info:\n", allIndent);
        printf("| %sfilename:\t\t%s\n", allIndent, model_data->model_vars_info_t[start_index+i].info.filename);
        printf("| %slineStart:\t\t%i\n", allIndent, model_data->model_vars_info_t[start_index+i].info.lineStart);
        printf("| %scolStart:\t\t%i\n", allIndent, model_data->model_vars_info_t[start_index+i].info.colStart);
        printf("| %slineEnd:\t\t%i\n", allIndent, model_data->model_vars_info_t[start_index+i].info.lineEnd);
        printf("| %scolEnd:\t\t\t%i\n", allIndent, model_data->model_vars_info_t[start_index+i].info.colEnd);
        printf("| %sfileWritable:\t\t%s\n", allIndent, model_data->model_vars_info_t[start_index+i].info.fileWritable ? "true" : "false");
        printf("%s\n", allIndent);
    }

    // free memory
    global_freeMemory(allIndent);

}
/*
 * debug function for development
 * ToDo: make prettier or delete when finished
 */
void omsu_print_debug (osu_t* OSU) {
    omsi_unsigned_int lastIndex, size;

    printf("\n========== omsu_print_debug start ==========\n");
    /* print omsi_data */
    printf("struct omsi_t:\n");
    // print model data
    printf("| struct model_data_t:\n");
    printf("| | modelGUID:\t\t\t%s\n", OSU->osu_data->model_data.modelGUID);
    printf("| | n_states:\t\t\t%d\n", OSU->osu_data->model_data.n_states);
    printf("| | n_derivatives:\t\t%d\n", OSU->osu_data->model_data.n_derivatives);
    printf("| | n_real_vars:\t\t%d\n", OSU->osu_data->model_data.n_real_vars);
    printf("| | n_int_vars:\t\t\t%d\n", OSU->osu_data->model_data.n_int_vars);
    printf("| | n_bool_vars:\t\t%d\n", OSU->osu_data->model_data.n_bool_vars);
    printf("| | n_string_vars:\t\t%d\n", OSU->osu_data->model_data.n_string_vars);
    printf("| | n_real_parameters:\t\t%d\n", OSU->osu_data->model_data.n_real_parameters);
    printf("| | n_int_parameters:\t\t%d\n", OSU->osu_data->model_data.n_int_parameters);
    printf("| | n_bool_parameters:\t\t%d\n", OSU->osu_data->model_data.n_bool_parameters);
    printf("| | n_string_parameters:\t%d\n", OSU->osu_data->model_data.n_string_parameters);
    printf("| | n_real_aliases:\t\t%d\n", OSU->osu_data->model_data.n_real_aliases);
    printf("| | n_int_aliases:\t\t%d\n", OSU->osu_data->model_data.n_int_aliases);
    printf("| | n_bool_aliases:\t\t%d\n", OSU->osu_data->model_data.n_bool_aliases);
    printf("| | n_string_aliases:\t\t%d\n", OSU->osu_data->model_data.n_string_aliases);
    printf("| | n_zerocrossings:\t\t%d\n", OSU->osu_data->model_data.n_zerocrossings);
    printf("| | n_equations:\t\t%d\n", OSU->osu_data->model_data.n_equations);

    printf("| | model_vars_info_t:\n");
    lastIndex = 0;
    size = OSU->osu_data->model_data.n_states + OSU->osu_data->model_data.n_derivatives
           + OSU->osu_data->model_data.n_real_vars + OSU->osu_data->model_data.n_real_parameters
           + OSU->osu_data->model_data.n_real_aliases;
    omsu_print_debug_helper(&(OSU->osu_data->model_data), lastIndex, size, 3);

    lastIndex += size;
    size = OSU->osu_data->model_data.n_int_vars + OSU->osu_data->model_data.n_int_parameters
           + OSU->osu_data->model_data.n_int_aliases;
    omsu_print_debug_helper(&(OSU->osu_data->model_data), lastIndex, size, 3);

    lastIndex += size;
    size = OSU->osu_data->model_data.n_bool_vars + OSU->osu_data->model_data.n_bool_parameters
           + OSU->osu_data->model_data.n_bool_aliases;
    omsu_print_debug_helper(&(OSU->osu_data->model_data), lastIndex, size, 3);

    lastIndex += size;
    size = OSU->osu_data->model_data.n_string_vars + OSU->osu_data->model_data.n_string_parameters
           + OSU->osu_data->model_data.n_string_aliases;
    omsu_print_debug_helper(&(OSU->osu_data->model_data), lastIndex, size, 3);

    printf("| | equation_info_t:\n");
    for(omsi_unsigned_int i=0; i<OSU->osu_data->model_data.n_equations; i++) {
        // print equation_info
    }

    // print sim_data


    // print experiment_data
    printf("|\n");
    printf("| struct omsi_experiment_t:\n");
    printf("| | start_time:\t\t\t%f\n", OSU->osu_data->experiment->start_time);
    printf("| | stop_time:\t\t\t%f\n", OSU->osu_data->experiment->stop_time);
    printf("| | step_size:\t\t\t%f\n", OSU->osu_data->experiment->step_size);
    printf("| | num_outputss:\t\t%u\n", OSU->osu_data->experiment->num_outputs);
    printf("| | tolerance:\t\t\t%1.e\n", OSU->osu_data->experiment->tolerance);
    printf("| | solver_name:\t\t%s\n", OSU->osu_data->experiment->solver_name);

    printf("\n==========  omsu_print_debug end  ==========\n\n");
}

omsi_int initializeNonlinearSystems(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
    UNUSED(data); UNUSED(threadData);
    return -1;
}

omsi_int initializeLinearSystems(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
    UNUSED(data); UNUSED(threadData);
    return -1;
}

omsi_int initializeMixedSystems(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
    UNUSED(data); UNUSED(threadData);
    return -1;
}

void initializeStateSetJacobians(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
    UNUSED(data); UNUSED(threadData);
}

void setZCtol(omsi_real relativeTol) {
    //ToDO: implement
    UNUSED(relativeTol);
}

void copyStartValuestoInitValues(DATA *data) {
    //ToDo: implement for new data structure
    UNUSED(data);
}

omsi_int initialization(DATA *data, threadData_t *threadData, omsi_string pInitMethod, omsi_string pInitFile, omsi_real initTime) {
    //ToDo: implement for new data structure
    UNUSED(data); UNUSED(threadData); UNUSED(pInitMethod); UNUSED(pInitFile); UNUSED(initTime);
    return -1;
}

void initSample(DATA* data, threadData_t *threadData, omsi_real startTime, omsi_real stopTime) {
    //ToDo: implement for new data structure
    UNUSED(data); UNUSED(threadData); UNUSED(startTime); UNUSED(stopTime);
}

void initDelay(DATA* data, omsi_real startTime) {
    //ToDo: implement for new data structure
    UNUSED(data); UNUSED(startTime);
}

omsi_real getNextSampleTimeFMU(DATA *data) {
    //ToDo: implement for new data structure
    UNUSED(data);
    return -1;
}

void setAllVarsToStart(DATA *data) {
    //ToDo: implement for new data structure
    UNUSED(data);
}

void setAllParamsToStart(DATA *data) {
    //ToDo: implement for new data structure
    UNUSED(data);
}

omsi_int freeNonlinearSystems(DATA *data, threadData_t *threadData) {
    //ToDo: implement for new data structure
    UNUSED(data); UNUSED(threadData);
    return -1;
}

omsi_int freeMixedSystems(DATA *data, threadData_t *threadData) {
    //ToDo: implement for new data structure
    UNUSED(data); UNUSED(threadData);
    return -1;
}

omsi_int freeLinearSystems(DATA *data, threadData_t *threadData) {
    //ToDo: implement for new data structure
    UNUSED(data); UNUSED(threadData);
    return -1;
}

void freeStateSetData(DATA *data) {
    //ToDo: implement for new data structure
    UNUSED(data);
}

void deInitializeDataStruc(DATA *data) {
    //ToDo: implement for new data structure
    UNUSED(data);
}

void updateRelationsPre(DATA *data) {
    //ToDo: implement for new data structure
    UNUSED(data);
}

modelica_boolean checkRelations(DATA *data) {
    //ToDo: implement for new data structure
    UNUSED(data);
    return omsi_false;
}

void mmc_catch_dummy_fn (void) {
    //ToDo: delete
}
