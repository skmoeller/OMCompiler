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

#define UNUSED(x) (void)(x)     /* ToDo: delete later */

/* forward global functions */
omsi_callback_allocate_memory   global_allocateMemory;
omsi_callback_free_memory       global_freeMemory;


/*
 * ============================================================================
 * Section for allocation and deallocation stuff
 * ============================================================================
 */

/*
 * frees memory for omsi_t struct and all its components
 */
void omsu_free_osu_data(omsi_t*                         omsi_data,
                        const omsi_callback_free_memory freeMemory) {

    omsi_unsigned_int i, j=0;
    omsi_unsigned_int size;

    real_var_attribute_t* attribute;

    /* free memory for model data */
    freeMemory((omsi_string)omsi_data->model_data->modelGUID);

    size = omsi_data->model_data->n_states + omsi_data->model_data->n_derivatives
         + omsi_data->model_data->n_real_vars+omsi_data->model_data->n_real_parameters
         + omsi_data->model_data->n_real_aliases;
    for (i=0; i<size; i++, j++) {
        freeMemory (omsi_data->model_data->model_vars_info_t[j].name);
        freeMemory (omsi_data->model_data->model_vars_info_t[j].comment);
        attribute = omsi_data->model_data->model_vars_info_t[j].modelica_attributes;
        freeMemory (attribute->unit);
        freeMemory (attribute->displayUnit);
        freeMemory (omsi_data->model_data->model_vars_info_t[j].modelica_attributes);
        /*freeMemory(omsi_data->model_data.model_vars_info_t[j].info.filename);     // ToDo: something is wrong here */
    }
    size += omsi_data->model_data->n_int_vars + omsi_data->model_data->n_int_parameters + omsi_data->model_data->n_int_aliases
            + omsi_data->model_data->n_bool_vars + omsi_data->model_data->n_bool_parameters + omsi_data->model_data->n_bool_aliases
            + omsi_data->model_data->n_string_vars + omsi_data->model_data->n_string_parameters + omsi_data->model_data->n_string_aliases;
    for (i=j; i<size; i++, j++) {
        freeMemory (omsi_data->model_data->model_vars_info_t[j].name);
        freeMemory (omsi_data->model_data->model_vars_info_t[j].comment);
        freeMemory (omsi_data->model_data->model_vars_info_t[j].modelica_attributes);
        /*freeMemory(omsi_data->model_data.model_vars_info_t[j].info.filename);     // ToDo: something is wrong here */
    }
    freeMemory (omsi_data->model_data->model_vars_info_t);

/*    for (i=0; i<omsi_data->model_data.n_equations; i++) {
        freeMemory (omsi_data->model_data.equation_info_t[i].variables);
    }
    //freeMemory (omsi_data->model_data.equation_info_t);     // ToDo: something's wrong here
*/

    /* free memory for simulation data */
    /* ToDo: free inner stuff of initialization */
    freeMemory (omsi_data->sim_data->initialization);
    /* ToDo: free inner stuff of initialization */
    freeMemory (omsi_data->sim_data->simulation);

    freeMemory (omsi_data->sim_data->model_vars_and_params->reals);
    freeMemory (omsi_data->sim_data->model_vars_and_params->ints);
    freeMemory (omsi_data->sim_data->model_vars_and_params->bools);

    freeMemory (omsi_data->sim_data->zerocrossings_vars);
    freeMemory (omsi_data->sim_data->pre_zerocrossings_vars);

    /* free memory for experiment data */
    freeMemory ((omsi_char *)omsi_data->experiment->solver_name);        /* type-cast to shut of warning when compiling */
    freeMemory (omsi_data->experiment);
}


/*
 * Allocates memory for sim_data_t struct and all its components.
 * Gets called from omsu_process_input_xml().
 */
omsi_status omsu_allocate_sim_data(omsi_t*                              omsi_data,
                                   const omsi_callback_allocate_memory  allocateMemory) {

    omsi_int n_model_vars_and_params;

    omsi_data->sim_data->initialization = (omsi_function_t*) allocateMemory(1, sizeof(omsi_function_t));
    if (!omsi_data->sim_data->initialization) {
        return omsi_error;      /* Error: Out of memory */
    }
    /* ToDo: add stuff, e.g. allocate memory for all parts of struct initialization */

    omsi_data->sim_data->simulation = (omsi_function_t*) allocateMemory(1, sizeof(omsi_function_t));
    if (!omsi_data->sim_data->initialization) {
        return omsi_error;      /* Error: Out of memory */
    }
    /* ToDo: add more stuff */

    n_model_vars_and_params = omsi_data->model_data->n_real_vars + omsi_data->model_data->n_int_vars + omsi_data->model_data->n_bool_vars + omsi_data->model_data->n_string_vars
                              + omsi_data->model_data->n_real_parameters + omsi_data->model_data->n_int_parameters + omsi_data->model_data->n_bool_parameters + omsi_data->model_data->n_string_parameters;
    omsi_data->sim_data->model_vars_and_params = (omsi_values *) allocateMemory(n_model_vars_and_params, sizeof(omsi_values));
    if (!omsi_data->sim_data->model_vars_and_params) {
        return omsi_error;      /* Error: Out of memory */
    }
    omsi_data->sim_data->model_vars_and_params->reals = (omsi_real *) allocateMemory(omsi_data->model_data->n_real_vars + omsi_data->model_data->n_real_parameters, sizeof(omsi_real));
    omsi_data->sim_data->model_vars_and_params->ints = (omsi_int*) allocateMemory(omsi_data->model_data->n_int_vars + omsi_data->model_data->n_int_parameters, sizeof(omsi_int));
    omsi_data->sim_data->model_vars_and_params->bools = (omsi_bool*) allocateMemory(omsi_data->model_data->n_bool_vars + omsi_data->model_data->n_bool_parameters, sizeof(omsi_bool));
    if (!omsi_data->sim_data->model_vars_and_params->reals || !omsi_data->sim_data->model_vars_and_params->ints || !omsi_data->sim_data->model_vars_and_params->bools ) {
        return omsi_error;      /* Error: Out of memory */
    }

    /* ToDo: allocate memory for some pre-values */

    omsi_data->sim_data->zerocrossings_vars = (omsi_bool *) allocateMemory(omsi_data->model_data->n_zerocrossings, sizeof(omsi_bool));
    omsi_data->sim_data->pre_zerocrossings_vars = (omsi_bool *) allocateMemory(omsi_data->model_data->n_zerocrossings, sizeof(omsi_bool));

    /* ToDo: Add error cases */

    return omsi_ok;
}



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
    nextIndent = (omsi_char*) global_allocateMemory(strlen(indent)+2, sizeof(omsi_char));
    strcat(nextIndent, "| ");

    /* print content of omsi_data */
    printf("%sstruct omsi_t:\n", indent);

    /* print model data */
    omsu_print_model_data (omsi->model_data, nextIndent);
    printf("%s\n", indent);

    /* print sim_data */
    omsu_print_sim_data(omsi->sim_data, nextIndent);
    printf("%s\n", indent);

    /* print experiment_data */
    omsu_print_experiment(omsi->experiment, nextIndent);
}


/*
 * Print all data in model_data_t structure.
 * Indent is string with indentation in form of "| " strings or "".
 */
void omsu_print_model_data(model_data_t*    model_data,
                           omsi_string      indent) {

    /* compute next indentation */
    omsi_char* nextIndent;
    nextIndent = (omsi_char*) global_allocateMemory(strlen(indent)+2, sizeof(omsi_char));
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

    /* print model_vars_info_t */
    omsu_print_model_variable_info(model_data, nextIndent);

    /* print equation_info_t */
    omsu_print_equation_info(model_data, nextIndent);

    /* free memory */
    global_freeMemory(nextIndent);
}


/*
 * Prints all model variables informations of array model_data->model_vars_info_t.
 * Indent is string with indentation in form of "| " strings or "".
 * Returns with omsi_warning if model_data is NULL pointer.
 */
omsi_status omsu_print_model_variable_info(model_data_t*  model_data,
                                           omsi_string    indent) {

    /* variables */
    omsi_unsigned_int i, size;
    omsi_char* nextnextIndent;

    /* compute next indentation */
    nextnextIndent = (omsi_char*) global_allocateMemory(strlen(indent)+4, sizeof(omsi_char));
    strcat(nextnextIndent, "| | ");

    if (model_data==NULL) {
        return omsi_error;
    }

    printf("%smodel_vars_info_t:\n", indent);

    if (model_data->model_vars_info_t==NULL) {
        printf("%s| No data\n", indent);
        return omsi_warning;
    }

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
        printf("%s| id:\t\t\t%i\n", indent, model_data->model_vars_info_t[i].id);
        printf("%s| name:\t\t\t%s\n", indent, model_data->model_vars_info_t[i].name);
        printf("%s| comment:\t\t\t%s\n", indent, model_data->model_vars_info_t[i].comment);
        printf("%s| variable type:\t\t%i\n", indent, (omsi_int)model_data->model_vars_info_t[i].type_index.type);
        printf("%s| variable index:\t\t%i\n", indent, model_data->model_vars_info_t[i].type_index.index);

        omsu_print_modelica_attributes(model_data->model_vars_info_t[i].modelica_attributes, &model_data->model_vars_info_t[i].type_index, nextnextIndent);

        printf("%s| alias:\t\t\t%s\n",  indent, model_data->model_vars_info_t[i].isAlias ? "true" : "false");
        printf("%s| negate:\t\t\t%i\n",  indent, model_data->model_vars_info_t[i].negate);
        printf("%s| aliasID:\t\t\t%i\n",  indent, model_data->model_vars_info_t[i].aliasID);

        printf("%s| file info:\n", indent);
        printf("| | %sfilename:\t\t%s\n", indent, model_data->model_vars_info_t[i].info.filename);
        printf("| | %slineStart:\t\t%i\n", indent, model_data->model_vars_info_t[i].info.lineStart);
        printf("| | %scolStart:\t\t%i\n", indent, model_data->model_vars_info_t[i].info.colStart);
        printf("| | %slineEnd:\t\t%i\n", indent, model_data->model_vars_info_t[i].info.lineEnd);
        printf("| | %scolEnd:\t\t\t%i\n", indent, model_data->model_vars_info_t[i].info.colEnd);
        printf("| | %sfileWritable:\t\t%s\n", indent, model_data->model_vars_info_t[i].info.fileWritable ? "true" : "false");
        printf("| %s\n", indent);
    }

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
    nextIndent = (omsi_char*) global_allocateMemory(strlen(indent)+2, sizeof(omsi_char));
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
        return omsi_error;
    }

    return omsi_ok;
}


/*
 * Print modelica attributes of real variable.
 * Indent is string with indentation in form of "| " strings or "".
 */
void omsu_print_real_var_attribute (real_var_attribute_t*   real_var_attribute,
                                    omsi_string             indent) {

    printf("%sunit:\t\t\t%s\n", indent, real_var_attribute->unit);
    printf("%sdisplayUnit:\t\t%s\n", indent, real_var_attribute->displayUnit);
    if (real_var_attribute->min <= -OMSI_DBL_MAX) {
        printf("%smin:\t\t\t-infinity\n", indent);
    }
    else {
        printf("%smin:\t\t\t%f\n", indent, real_var_attribute->min);
    }
    if (real_var_attribute->max >= OMSI_DBL_MAX) {
        printf("%smax:\t\t\tininity\n", indent);
    }
    else {
        printf("%smax:\t\t\t%f\n", indent, real_var_attribute->max);
    }
    printf("%sfixed:\t\t\t%s\n", indent, real_var_attribute->fixed ? "true" : "false");
    printf("%snominal:\t\t%f\n", indent, real_var_attribute->nominal);
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
 * Prints all equation informations of model_data->equation_info_t.
 * Indent is string with indentation in form of "| " strings or "".
 * Returns with omsi_warning if equation_info_t is NULL pointer.
 */
omsi_status omsu_print_equation_info(model_data_t*  model_data,
                                     omsi_string    indent) {

    /* Variables */
    omsi_unsigned_int i;
    omsi_int j;

    if (model_data==NULL) {
        return omsi_error;
    }

    printf("%sequation_info_t:\n", indent);

    if (model_data->equation_info_t==NULL) {
        printf("%s| No data\n", indent);
        return omsi_warning;
    }

    for(i=0; i<model_data->n_equations; i++) {
        printf("%s| id:\t\t\t%i\n", indent, model_data->equation_info_t[i].id);
        printf("%s| ProfileBlockIndex:\t%i\n", indent, model_data->equation_info_t[i].profileBlockIndex);
        printf("%s| parent: \t\t\t%i\n", indent, model_data->equation_info_t[i].parent);
        printf("%s| numVar:\t\t\t%i\n", indent, model_data->equation_info_t[i].numVar);
        printf("%s| variables:\t\t", indent);
        for (j=0; j<model_data->equation_info_t[i].numVar; j++) {
            printf("%s%s ", indent, model_data->equation_info_t[i].variables[j]);
        }
        printf("\n");
        printf("%s| file info:\n", indent);
        printf("%s| | filename:\t\t%s\n", indent, model_data->equation_info_t[i].info.filename);
        printf("%s| | lineStart:\t\t%i\n", indent, model_data->equation_info_t[i].info.lineStart);
        printf("%s| | colStart:\t\t%i\n", indent, model_data->equation_info_t[i].info.colStart);
        printf("%s| | lineEnd:\t\t%i\n", indent, model_data->equation_info_t[i].info.lineEnd);
        printf("%s| | colEnd:\t\t\t%i\n", indent, model_data->equation_info_t[i].info.colEnd);
        printf("%s| | fileWritable:\t\t%s\n", indent, model_data->equation_info_t[i].info.fileWritable ? "true" : "false");
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
void omsu_print_sim_data (sim_data_t* sim_data,
                          omsi_string indent) {

    printf("%sstruct sim_data:\n", indent);

    /* ToDo: print sim_data */

}
