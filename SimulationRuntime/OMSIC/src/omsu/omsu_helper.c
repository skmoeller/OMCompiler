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

#include <omsu_helper.h>

#define UNUSED(x) (void)(x)     /* ToDo: delete later */




/*
 * ============================================================================
 * Section for print and debug functions
 * ============================================================================
 */


/*
 * Print all data in osu_t structure.
 */
void omsu_print_osu (osu_t* OSU) {

    printf("\n========== omsu_print_osu start ==========\n");

    omsu_print_omsi_t(OSU->osu_data, "");

    /* ToDO: Print rest of OSU */

    printf("\n==========  omsu_print_osu end  ==========\n\n");
}


/*
 * Print all data in omsi_t structure.
 * Indent is string with indentation in form of "| " strings or "".
 */
void omsu_print_omsi_t (omsi_t*     omsi,
                        omsi_string indent) {

    /* compute next indentation */
    omsi_char* nextIndent;
    nextIndent = (omsi_char*) global_callback->allocateMemory(strlen(indent)+2, sizeof(omsi_char));
    strcat(nextIndent, "| ");

    /* print content of omsi_data */
    printf("%sstruct omsi_t:\n", indent); fflush(stdout);

    /* print model data */
    omsu_print_model_data (omsi->model_data, nextIndent);
    printf("%s\n", indent);

    /* print sim_data */
    omsu_print_sim_data(omsi->sim_data, nextIndent);
    printf("%s\n", indent);

    /* print experiment_data */
    omsu_print_experiment(omsi->experiment, nextIndent);

    /* free memory */
    global_callback->freeMemory(nextIndent);
}


/*
 * Print all data in model_data_t structure.
 * Indent is string with indentation in form of "| " strings or "".
 */
void omsu_print_model_data(model_data_t*    model_data,
                           omsi_string      indent) {

    /* compute next indentation */
    omsi_char* nextIndent;
    nextIndent = (omsi_char*) global_callback->allocateMemory(strlen(indent)+2, sizeof(omsi_char));
    strcat(nextIndent, "| ");

    /* print content */
    printf("%sstruct model_data_t:\n", indent);
    printf("%s| modelGUID:\t\t\t%s\n", indent, model_data->modelGUID);
    printf("%s| n_states:\t\t\t%d\n", indent, model_data->n_states);
    printf("%s| n_derivatives:\t\t%d\n", indent, model_data->n_derivatives);
    printf("%s| n_real_vars:\t\t%d\n", indent, model_data->n_real_vars);
    printf("%s| n_int_vars:\t\t\t%d\n", indent, model_data->n_int_vars);
    printf("%s| n_bool_vars:\t\t%d\n", indent, model_data->n_bool_vars);
    printf("%s| n_string_vars:\t\t%d\n", indent, model_data->n_string_vars);
    printf("%s| n_real_parameters:\t\t%d\n", indent, model_data->n_real_parameters);
    printf("%s| n_int_parameters:\t\t%d\n", indent, model_data->n_int_parameters);
    printf("%s| n_bool_parameters:\t\t%d\n", indent, model_data->n_bool_parameters);
    printf("%s| n_string_parameters:\t%d\n", indent, model_data->n_string_parameters);
    printf("%s| n_real_aliases:\t\t%d\n", indent, model_data->n_real_aliases);
    printf("%s| n_int_aliases:\t\t%d\n", indent, model_data->n_int_aliases);
    printf("%s| n_bool_aliases:\t\t%d\n", indent, model_data->n_bool_aliases);
    printf("%s| n_string_aliases:\t\t%d\n", indent, model_data->n_string_aliases);
    printf("%s| n_zerocrossings:\t\t%d\n", indent, model_data->n_zerocrossings);
    printf("%s| n_regular_equations:\t%d\n", indent, model_data->n_regular_equations);
    printf("%s| n_init_equations:\t\t%d\n", indent, model_data->n_init_equations);
    printf("%s| n_alias_equations:\t\t%d\n", indent, model_data->n_alias_equations);
    printf("%s| n_equations:\t\t%d\n", indent, model_data->n_equations);

    /* print model_vars_info */
    omsu_print_model_variable_info(model_data, nextIndent);

    /* print equation_info */
    omsu_print_equation_info(model_data, nextIndent);

    /* free memory */
    global_callback->freeMemory(nextIndent);
}


/*
 * Prints all model variables informations of array model_data->model_vars_info.
 * Indent is string with indentation in form of "| " strings or "".
 * Returns with omsi_warning if model_data is NULL pointer.
 */
omsi_status omsu_print_model_variable_info(model_data_t*  model_data,
                                           omsi_string    indent) {

    /* variables */
    omsi_unsigned_int i, size;
    omsi_char* nextnextIndent;

    printf("%smodel_vars_info:\n", indent);

    if (model_data==NULL) {
        printf("%s| No model_data\n", indent);
        return omsi_error;
    }

    if (model_data->model_vars_info==NULL) {
        printf("%s| No model_vars_info\n", indent);
        return omsi_warning;
    }

    /* compute next indentation */
    nextnextIndent = (omsi_char*) global_callback->allocateMemory(strlen(indent)+4, sizeof(omsi_char));
    strcat(nextnextIndent, "| | ");

    /* print variables */
    size = model_data->n_states + model_data->n_derivatives
           + model_data->n_real_vars + model_data->n_real_parameters
           + model_data->n_real_aliases
           + model_data->n_int_vars + model_data->n_int_parameters
           + model_data->n_int_aliases
           + model_data->n_bool_vars + model_data->n_bool_parameters
           + model_data->n_bool_aliases
           + model_data->n_string_vars + model_data->n_string_parameters
           + model_data->n_string_aliases;

    for (i=0; i<size; i++) {
        printf("%s| id:\t\t\t\t%i\n", indent, model_data->model_vars_info[i].id);
        printf("%s| name:\t\t\t%s\n", indent, model_data->model_vars_info[i].name);
        printf("%s| comment:\t\t\t%s\n", indent, model_data->model_vars_info[i].comment);
        printf("%s| variable type:\t\t%s\n", indent, omsiDataTypesNames[model_data->model_vars_info[i].type_index.type]);
        printf("%s| variable index:\t\t%i\n", indent, model_data->model_vars_info[i].type_index.index);

        omsu_print_modelica_attributes(model_data->model_vars_info[i].modelica_attributes, &model_data->model_vars_info[i].type_index, nextnextIndent);

        printf("%s| alias:\t\t\t%s\n",  indent, model_data->model_vars_info[i].isAlias ? "true" : "false");
        printf("%s| negate:\t\t\t%i\n",  indent, model_data->model_vars_info[i].negate);
        printf("%s| aliasID:\t\t\t%i\n",  indent, model_data->model_vars_info[i].aliasID);

        printf("%s| file info:\n", indent);
        printf("| | %sfilename:\t\t\t%s\n", indent, model_data->model_vars_info[i].info.filename);
        printf("| | %slineStart:\t\t%i\n", indent, model_data->model_vars_info[i].info.lineStart);
        printf("| | %scolStart:\t\t\t%i\n", indent, model_data->model_vars_info[i].info.colStart);
        printf("| | %slineEnd:\t\t\t%i\n", indent, model_data->model_vars_info[i].info.lineEnd);
        printf("| | %scolEnd:\t\t\t%i\n", indent, model_data->model_vars_info[i].info.colEnd);
        printf("| | %sfileWritable:\t\t%s\n", indent, model_data->model_vars_info[i].info.fileWritable ? "true" : "false");
        printf("| %s\n", indent);
    }

    /* free memory */
    global_callback->freeMemory(nextnextIndent);
    return omsi_ok;
}


/*
 * Print modelica attribute.
 * Type_index determines the data type of the modelica attribute (real|int|bool|string).
 * Indent is string with indentation in form of "| " strings or "".
 * Returns with omsi_error if data type is not one of real, int, bool or string.
 */
omsi_status omsu_print_modelica_attributes (void*               modelica_attribute,
                                            omsi_index_type*    type_index,
                                            omsi_string         indent) {

    /* variables */
    real_var_attribute_t* attribute_real;
    int_var_attribute_t* attribute_integer;
    bool_var_attribute_t* attribute_bool;
    string_var_attribute_t* attribute_string;
    omsi_char* nextIndent;

    /* compute next indentation */
    nextIndent = (omsi_char*) global_callback->allocateMemory(strlen(indent)+2, sizeof(omsi_char));
    strcat(nextIndent, "| ");

    printf("%sattribute:\n", indent);

    switch (type_index->type) {
    case OMSI_TYPE_REAL:
        attribute_real = modelica_attribute;
        omsu_print_real_var_attribute(attribute_real, nextIndent);
        break;
    case OMSI_TYPE_INTEGER:
        attribute_integer = modelica_attribute;
        omsu_printf_int_var_attribute(attribute_integer, nextIndent);
        break;
    case OMSI_TYPE_BOOLEAN:
        attribute_bool = modelica_attribute;
        printf("| %sfixed:\t\t\t%s\n", indent, attribute_bool->fixed ? "true" : "false");
        printf("| %sstart:\t\t\t%s\n", indent, attribute_bool->start ? "true" : "false");
        break;
    case OMSI_TYPE_STRING:
        attribute_string = modelica_attribute;
        printf("| %sstart:\t\t\t%s\n", indent, attribute_string->start);
        break;
    default:
        /* free memory */
        global_callback->freeMemory(nextIndent);
        return omsi_error;
    }

    /* free memory */
    global_callback->freeMemory(nextIndent);
    return omsi_ok;
}


/*
 * Print modelica attributes of real variable.
 * Indent is string with indentation in form of "| " strings or "".
 */
void omsu_print_real_var_attribute (real_var_attribute_t*   real_var_attribute,
                                    omsi_string             indent) {

    printf("%sunit:\t\t\t\t%s\n", indent, real_var_attribute->unit);
    printf("%sdisplayUnit:\t\t%s\n", indent, real_var_attribute->displayUnit);
    if (real_var_attribute->min <= -OMSI_DBL_MAX) {
        printf("%smin:\t\t\t\t-infinity\n", indent);
    }
    else {
        printf("%smin:\t\t\t\t%f\n", indent, real_var_attribute->min);
    }
    if (real_var_attribute->max >= OMSI_DBL_MAX) {
        printf("%smax:\t\t\t\tinfinity\n", indent);
    }
    else {
        printf("%smax:\t\t\t\t%f\n", indent, real_var_attribute->max);
    }
    printf("%sfixed:\t\t\t%s\n", indent, real_var_attribute->fixed ? "true" : "false");
    printf("%snominal:\t\t\t%f\n", indent, real_var_attribute->nominal);
    printf("%sstart:\t\t\t%f\n", indent, real_var_attribute->start);
}


/*
 * Print modelica attributes of integer variable.
 * Indent is string with indentation in form of "| " strings or "".
 */
void omsu_printf_int_var_attribute(int_var_attribute_t* int_var_attribute,
                                   omsi_string          indent) {

    if (int_var_attribute->min <= -OMSI_INT_MAX) {
        printf("%smin:\t\t\t-infinity\n", indent);
    }
    else {
        printf("%smin:\t\t\t%i\n", indent, int_var_attribute->min);
    }
    if (int_var_attribute->max >= OMSI_INT_MAX) {
        printf("%smax:\t\t\tininity\n", indent);
    }
    else {
        printf("%smax:\t\t\t%i\n", indent, int_var_attribute->max);
    }
    printf("%sfixed:\t\t\t%s\n", indent, int_var_attribute->fixed ? "true" : "false");
    printf("%sstart:\t\t\t%i\n", indent, int_var_attribute->start);
}


/*
 * Prints all equation informations of model_data->equation_info.
 * Indent is string with indentation in form of "| " strings or "".
 * Returns with omsi_warning if equation_info is NULL pointer.
 */
omsi_status omsu_print_equation_info(model_data_t*  model_data,
                                     omsi_string    indent) {

    /* Variables */
    omsi_unsigned_int i;
    omsi_int j;

    if (model_data==NULL) {
        return omsi_error;
    }

    printf("%sequation_info:\n", indent);

    if (model_data->equation_info==NULL) {
        printf("%s| No data\n", indent);
        return omsi_warning;
    }

    for(i=0; i<model_data->n_equations; i++) {
        printf("%s| id:\t\t\t\t%i\n", indent, model_data->equation_info[i].id);
        printf("%s| ProfileBlockIndex:\t\t%i\n", indent, model_data->equation_info[i].profileBlockIndex);
        printf("%s| parent: \t\t\t%i\n", indent, model_data->equation_info[i].parent);
        printf("%s| numVar:\t\t\t%i\n", indent, model_data->equation_info[i].numVar);
        printf("%s| variables:\t\t\t", indent);
        for (j=0; j<model_data->equation_info[i].numVar; j++) {
            printf("%s ", model_data->equation_info[i].variables[j]);
        }
        printf("\n");
        printf("%s| file info:\n", indent);
        printf("%s| | filename:\t\t\t%s\n", indent, model_data->equation_info[i].info.filename);
        printf("%s| | lineStart:\t\t%i\n", indent, model_data->equation_info[i].info.lineStart);
        printf("%s| | colStart:\t\t\t%i\n", indent, model_data->equation_info[i].info.colStart);
        printf("%s| | lineEnd:\t\t\t%i\n", indent, model_data->equation_info[i].info.lineEnd);
        printf("%s| | colEnd:\t\t\t%i\n", indent, model_data->equation_info[i].info.colEnd);
        printf("%s| | fileWritable:\t\t%s\n", indent, model_data->equation_info[i].info.fileWritable ? "true" : "false");
        printf("%s\n", indent);
    }

    return omsi_ok;
}


/*
 * Print all data in omsi_experiment_t structure.
 * Indent is string with indentation in form of "| " strings or "".
 */
void omsu_print_experiment (omsi_experiment_t*  experiment,
                            omsi_string         indent) {

    printf("%sstruct omsi_experiment_t:\n", indent);
    printf("%s| start_time:\t\t\t%f\n", indent, experiment->start_time);
    printf("%s| stop_time:\t\t\t%f\n", indent, experiment->stop_time);
    printf("%s| step_size:\t\t\t%f\n", indent, experiment->step_size);
    printf("%s| num_outputss:\t\t%u\n", indent, experiment->num_outputs);
    printf("%s| tolerance:\t\t\t%1.e\n", indent, experiment->tolerance);
    printf("%s| solver_name:\t\t%s\n", indent, experiment->solver_name);
}


/*
 * Print all data in sim_data_t structure.
 * Indent is string with indentation in form of "| " strings or "".
 */
omsi_status omsu_print_sim_data (sim_data_t* sim_data,
                                 omsi_string indent) {

    omsi_char* nextIndent;

    if (sim_data==NULL) {
        printf("%sNo sim_data\n",indent);
        return omsi_warning;
    }

    /* compute next indentation */
    nextIndent = (omsi_char*) global_callback->allocateMemory(strlen(indent)+2, sizeof(omsi_char));
    strcat(nextIndent, "| ");


    printf("%sstruct sim_data:\n", indent);

    omsu_print_omsi_function_rec (sim_data->initialization, "initialization", nextIndent);

    omsu_print_omsi_function_rec (sim_data->simulation, "simulation", nextIndent);

    omsu_print_omsi_values(sim_data->model_vars_and_params, "model_vars_and_params", nextIndent);

    omsu_print_omsi_values(sim_data->pre_vars, "pre_vars", nextIndent);



    /* ToDo: print rest of sim_data */

    /* free memory */
    global_callback->freeMemory(nextIndent);
    return omsi_ok;
}


/*
 * Print all data in omsi_function_t structure and its containing structures.
 * Indent is string with indentation in form of "| " strings or "".
 */
omsi_status omsu_print_omsi_function_rec (omsi_function_t* omsi_function,
                                          omsi_string      omsi_function_name,
                                          omsi_string      indent) {

    /* Variables */
    omsi_unsigned_int i;
    omsi_char* nextIndent;

    if (omsi_function==NULL) {
        printf("%sNo omsi_function_t %s\n",indent, omsi_function_name);
        return omsi_warning;
    }
    else {
        printf("%somsi_function_t %s\n",indent, omsi_function_name);
    }

    /* compute next indentation */
    nextIndent = (omsi_char*) global_callback->allocateMemory(strlen(indent)+2, sizeof(omsi_char));
    strcat(nextIndent, "| ");

    printf("%sn_algebraic_system:\t\t%u\n", indent, omsi_function->n_algebraic_system);
    for (i=0; i<omsi_function->n_algebraic_system; i++) {
        omsu_print_algebraic_system(&omsi_function->algebraic_system_t[i], nextIndent);
    }
    omsu_print_this_omsi_function (omsi_function, omsi_function_name, indent);

    /* free memory */
    global_callback->freeMemory(nextIndent);
    return omsi_ok;
}


/*
 * Print all data in this omsi_function_t structure except algebraic systems.
 * Indent is string with indentation in form of "| " strings or "".
 */
omsi_status omsu_print_this_omsi_function (omsi_function_t* omsi_function,
                                           omsi_string      omsi_function_name,
                                           omsi_string      indent) {

    /* Variables */
    omsi_char* nextIndent;

    if (omsi_function==NULL) {
        printf("%sNo omsi_function_t %s\n",indent, omsi_function_name);
        return omsi_warning;
    }

    /* compute next indentation */
    nextIndent = (omsi_char*) global_callback->allocateMemory(strlen(indent)+2, sizeof(omsi_char));
    strcat(nextIndent, "| ");

    omsu_print_omsi_values(omsi_function->function_vars, "function_vars", indent);

    printf("%sevaluate function pointer set: %s\n", indent, omsi_function->evaluate!=NULL? "true" : "false");

    omsu_print_index_type(omsi_function->input_vars_indices, omsi_function->n_input_vars, nextIndent);
    omsu_print_index_type(omsi_function->input_vars_indices, omsi_function->n_output_vars, nextIndent);

    printf("%sn_input_vars:\t\t\t%i\n", indent, omsi_function->n_input_vars);
    printf("%sn_inner_vars:\t\t\t%i\n", indent, omsi_function->n_inner_vars);
    printf("%sn_output_vars:\t\t%i\n", indent, omsi_function->n_output_vars);

    /* free memory */
    global_callback->freeMemory(nextIndent);
    return omsi_ok;
}


/*
 * Print all values of omsi_values.
 */
omsi_status omsu_print_omsi_values (omsi_values*        omsi_values,
                                    omsi_string         omsi_values_name,
                                    omsi_string         indent) {

    omsi_unsigned_int i;

    if (omsi_values==NULL) {
        printf("%sNo omsi_values %s\n",indent, omsi_values_name);
        return omsi_warning;
    }

    printf("%somsi_values %s\n",indent, omsi_values_name);
    printf("%sreals:\n", indent);
    for(i=0; i<omsi_values->n_reals; i++) {
        printf("%s| \t\t\t\t%f\n", indent, omsi_values->reals[i]);
    }

    printf("%sints:\n", indent);
    for(i=0; i<omsi_values->n_ints; i++) {
        printf("%s| \t\t\t\t%i\n", indent, omsi_values->ints[i]);
    }

    printf("%sbools:\n", indent);
    for(i=0; i<omsi_values->n_bools; i++) {
        printf("%s| \t\t\t\t%s\n", indent, omsi_values->bools[i] ? "true" : "false");
    }

    /* omsu_print_externs(omsi_values->externs, omsi_values->n_externs); */

    printf("%stime_value: \t\t\t%f\n", indent, omsi_values->time_value);

    return omsi_ok;
}


/*
 * Print all informations of algebraic_system_t structure.
 */
omsi_status omsu_print_algebraic_system(omsi_algebraic_system_t*    algebraic_system_t,
                                        omsi_string                 indent) {

    omsi_unsigned_int i;
    omsi_char* nextIndent;

    /* compute next indentation */
    nextIndent = (omsi_char*) global_callback->allocateMemory(strlen(indent)+2, sizeof(omsi_char));
    strcat(nextIndent, "| ");

    printf("%sn_iteration_vars %u\n", indent, algebraic_system_t->n_iteration_vars);
    printf("%sn_conditions %u\n", indent, algebraic_system_t->n_conditions);

    printf("%szerocrossing indices; ", indent);
    for (i=0; i<algebraic_system_t->n_conditions; i++) {
        printf("%s| %i",indent, algebraic_system_t->zerocrossing_indices[i]);
    }
    printf("\n");

    printf("%sis linear: %s", indent, algebraic_system_t->isLinear ? "true" : "false");

    omsu_print_this_omsi_function(algebraic_system_t->jacobian, "jacobian", nextIndent);
    omsu_print_this_omsi_function(algebraic_system_t->functions, "residual functions", nextIndent);

    omsu_print_solver_data("lapack_solver", algebraic_system_t->solver_data, nextIndent);

    /* free memory */
    global_callback->freeMemory(nextIndent);

    return omsi_ok;
}


/* ToDo: finish function */
omsi_status omsu_print_index_type (omsi_index_type*     vars_indices,
                                   omsi_unsigned_int    size,
                                   omsi_string          indent) {

    UNUSED(vars_indices); UNUSED(size); UNUSED(indent);
    return omsi_ok;
}


/* ToDo: finish function */
omsi_status omsu_print_externs(void*                externs,
                               omsi_unsigned_int    n_externs) {

    UNUSED(externs); UNUSED(n_externs);

    printf("ERROR: omsu_print_externs not implemented yet\n");
    return omsi_error;
}


omsi_status omsu_print_solver_data(omsi_string  solver_name,
                                   void*        solver_data,
                                   omsi_string  indent) {

    UNUSED(solver_data); UNUSED(indent);

    if (strcmp("lapack_solver", solver_name)==0) {
        /* printLapackData(solver_data, indent); ToDo Uncomment*/
    }
    else {
        printf("WARING in function omsu_print_solver_data: type of solver_data unknown\n");
        return omsi_warning;
    }

    return omsi_ok;
}

