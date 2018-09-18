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

#include <omsi_input_sim_data.h>

#define UNUSED(x) (void)(x)     /* ToDo: delete later */

/*
 * Allocates memory and initializes sim_data_t struct with functions from generated code.
 * Assumes memory is already allocated for omsi_data->sim_data.
 */
omsi_status omsu_setup_sim_data(omsi_t*                             omsi_data,
                                omsi_template_callback_functions_t* template_function,
                                const omsi_callback_functions*      callback_functions) {

    /* set global function pointer */
    global_callback = callback_functions;

    global_callback->logger(global_callback->componentEnvironment, "instanceName", omsi_ok, "info", "Process sim data.");    /* ToDo: add instance name */

    /* check if template callback functions are set */
    if (!template_function->isSet) {
        /* ToDo: Log error */
        return omsi_error;
    }

    /* Call generated initialization function for initialization problem */
    /* osu_functions->initialize_initialization_problem(omsi_data->sim_data->initialization); */ /* ToDo: not implemented yet */

    /* Call generated initialization function for initialization problem */
    if (!template_function->initialize_simulation_problem(omsi_data->sim_data->simulation)) {
        /* ToDo: Log error */
        return omsi_error;
    }

    if (!omsu_instantiate_omsi_function(omsi_data->sim_data->simulation)) {
        /* ToDo: Log error */
        return omsi_error;
    }

    /* Set local function_vars to global model_vars_and_params */
    omsi_data->sim_data->simulation->function_vars = omsi_data->sim_data->model_vars_and_params;


    /* Set model_vars_and_params */
    if (!omsu_set_model_vars_and_params_start(omsi_data->sim_data->model_vars_and_params, omsi_data->model_data)) {
        /* ToDo: Log error */
        return omsi_error;
    }


    return omsi_ok;
}

/*
 * Allocates memory for sim_data_t struct.
 * Gets called from function omsu_setup_sim_data.
 */
omsi_status omsu_allocate_sim_data(omsi_t*                          omsu,
                                   const omsi_callback_functions*   callback_functions,
                                   omsi_string                      instanceName ) {

    /* set global function pointer */
    global_callback = callback_functions;


    omsu->sim_data = (sim_data_t*)global_callback->allocateMemory(1, sizeof(sim_data_t));
    if (!omsu->sim_data) {
        callback_functions->logger(callback_functions->componentEnvironment, instanceName, omsi_error, "error",
            "omsu_allocate_sim_data: Not enough memory.");
        return -1;
    }

    omsu->sim_data->model_vars_and_params = (omsi_values*)global_callback->allocateMemory(1, sizeof(omsi_values));
    if (!omsu->sim_data->model_vars_and_params) {
        callback_functions->logger(callback_functions->componentEnvironment, instanceName, omsi_error, "error",
            "omsu_allocate_sim_data: Not enough memory.");
        return -1;
    }

    omsu->sim_data->pre_vars = (omsi_values*)global_callback->allocateMemory(1, sizeof(omsi_values));
    if (!omsu->sim_data->pre_vars) {
        callback_functions->logger(callback_functions->componentEnvironment, instanceName, omsi_error, "error",
            "omsu_allocate_sim_data: Not enough memory.");
        return -1;
    }



    //omsi_unsigned_int n_reals, n_ints, n_bools, n_externs;

    //omsi_data->sim_data->initialization = (omsi_function_t*) global_callback->allocateMemory(1, sizeof(omsi_function_t));
    //if (!omsi_data->sim_data->initialization) {
    //    return omsi_error;      /* Error: Out of memory */
    //}

    //omsi_data->sim_data->simulation = (omsi_function_t*) global_callback->allocateMemory(1, sizeof(omsi_function_t));
    //if (!omsi_data->sim_data->initialization) {
    //    return omsi_error;      /* Error: Out of memory */
    //}

    ///* Allocate memory for model_vars_and_params */
    //n_reals = omsi_data->model_data->n_real_vars + omsi_data->model_data->n_real_parameters;
    //n_ints = omsi_data->model_data->n_int_vars + omsi_data->model_data->n_int_parameters;
    //n_bools = omsi_data->model_data->n_bool_vars + omsi_data->model_data->n_bool_parameters;
    //n_externs = 0;

    //omsi_data->sim_data->model_vars_and_params = instantiate_omsi_values (n_reals, n_ints, n_bools, n_externs);
    //if (!omsi_data->sim_data->model_vars_and_params) {
    //    return omsi_error;      /* Error: Out of memory */
    //}

    ///* ToDo: allocate memory for some pre-values */

    //omsi_data->sim_data->zerocrossings_vars = (omsi_bool *) global_callback->allocateMemory(omsi_data->model_data->n_zerocrossings, sizeof(omsi_bool));
    //omsi_data->sim_data->pre_zerocrossings_vars = (omsi_bool *) global_callback->allocateMemory(omsi_data->model_data->n_zerocrossings, sizeof(omsi_bool));

    /* ToDo: Add error cases */

    return omsi_ok;
}


/*
 * Instantiate omsi_function_t structure.
 */
omsi_status omsu_instantiate_omsi_function (omsi_function_t* omsi_function) {

    UNUSED(omsi_function);
    return omsi_error;
}


/*
 * Allocates memory for omsi_algebraic_system_t struct.
 * Since n_conditions is unknown memory for zerocrossing_indices is not allocated!
 */
omsi_algebraic_system_t* omsu_initialize_alg_system_array (omsi_unsigned_int n_algebraic_system) {

    /* Variables */
    omsi_algebraic_system_t* algebraic_system;

    /* allocate memory */
    algebraic_system = (omsi_algebraic_system_t*) global_callback->allocateMemory(n_algebraic_system, sizeof(omsi_algebraic_system_t));

    if (!algebraic_system) {
        /* ToDo: Log error */
        return NULL;
    }

    return algebraic_system;
}


/*
 * Set start values for model_vars_and_params from model_data.
 * If no start value was specified in original modelica model it defaults to zero.
 */
omsi_status omsu_set_model_vars_and_params_start (omsi_values*     model_vars_and_params,
                                                  model_data_t*    model_data) {

    /* Variables */
    omsi_unsigned_int i;
    omsi_unsigned_int length;

    real_var_attribute_t* real_attribute;
    int_var_attribute_t* int_attribute;
    bool_var_attribute_t* bool_attribute;

    length = model_data->n_states + model_data->n_derivatives
           + model_data->n_real_vars + model_data->n_real_parameters
           + model_data->n_real_aliases
           + model_data->n_int_vars + model_data->n_int_parameters
           + model_data->n_int_aliases
           + model_data->n_bool_vars + model_data->n_bool_parameters
           + model_data->n_bool_aliases
           + model_data->n_string_vars + model_data->n_string_parameters
           + model_data->n_string_aliases;

    /* Read start values from model_vars_info_t and write them into model_vars_and_params */
    for (i=0; i<length; i++) {
        if (!model_data->model_vars_info_t[i].isAlias) {
            switch (model_data->model_vars_info_t[i].type_index.type) {
            case OMSI_TYPE_REAL:
                real_attribute = (real_var_attribute_t*) model_data->model_vars_info_t[i].modelica_attributes;
                model_vars_and_params->reals[model_data->model_vars_info_t[i].type_index.index]
                         = real_attribute->start;
                break;
            case OMSI_TYPE_INTEGER:
                int_attribute = (int_var_attribute_t*) model_data->model_vars_info_t[i].modelica_attributes;
                model_vars_and_params->ints[model_data->model_vars_info_t[i].type_index.index]
                         = int_attribute->start;
                break;
            case OMSI_TYPE_BOOLEAN:
                bool_attribute = (bool_var_attribute_t*) model_data->model_vars_info_t[i].modelica_attributes;
                model_vars_and_params->bools[model_data->model_vars_info_t[i].type_index.index]
                         = bool_attribute->start;
                break;
            case OMSI_TYPE_STRING:
            default:
                /*ToDo: Log error */
                return omsi_error;
            }
        }
    }

    return omsi_ok;
}





/*
 * Allocates memory for omsi_values struct.
 * Needs length for arrays reals, ints, bools and externs as input.
 * Returns NULL in error case.
 */
omsi_values* instantiate_omsi_values (omsi_unsigned_int   n_reals,    /* length of array reals */
                                      omsi_unsigned_int   n_ints,     /* length of array ints */
                                      omsi_unsigned_int   n_bools,    /* length of array bools */
                                      omsi_unsigned_int   n_externs){ /* length of array externs */

    omsi_values* values;

    /* catch not implemented case*/
    if (n_externs > 0) {
        /* ToDo: Log error, not implemented yet */
        return NULL;
    }

    /* Allocate memory */
    values = (omsi_values*) global_callback->allocateMemory(1, sizeof(omsi_values));

    values->reals = (omsi_real*) global_callback->allocateMemory(n_reals, sizeof(omsi_real));
    values->ints = (omsi_int*) global_callback->allocateMemory(n_ints, sizeof(omsi_int));
    values->bools = (omsi_bool*) global_callback->allocateMemory(n_bools, sizeof(omsi_bool));

    /* check for out of memory error */
    if (!values || !values->reals || !values->ints || !values->bools) {
        /* ToDo: Log error out of memory */
        return NULL;
    }

    return values;
}
