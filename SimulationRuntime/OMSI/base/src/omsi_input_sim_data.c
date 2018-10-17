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
    global_callback = (omsi_callback_functions*) callback_functions;

    filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2Instantiate: Set up sim_data structure.");

    /* Check if memory is already allocated */
    if (!omsi_data->sim_data) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: sim_data struct not allocated.");
        return omsi_error;
    }

    /* check if template callback functions are set */
    if (!template_function->isSet) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: Generated functions not set.");
        return omsi_error;
    }

    /* Call generated initialization function for initialization problem */
    /* osu_functions->initialize_initialization_problem(omsi_data->sim_data->initialization); */ /* ToDo: not implemented yet */

    /* Call generated initialization function for initialization problem */
    if (template_function->initialize_simulation_problem(omsi_data->sim_data->simulation)==omsi_error) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: Error while instantiating initialization problem with generated code.");
        return omsi_error;
    }


    if (omsu_instantiate_omsi_function_func_vars(omsi_data->sim_data->simulation, omsi_data->sim_data->model_vars_and_params)==omsi_error) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: Error while instantiating function variables of sim_data->simulation.");
        return omsi_error;
    }


    /* Set model_vars_and_params */

    return omsi_ok;
}

/*
 * Allocates memory for sim_data_t struct.
 * Gets called from function omsu_setup_sim_data.
 */
omsi_status omsu_allocate_sim_data(omsi_t*                          omsu,
                                   const omsi_callback_functions*   callback_functions,
                                   omsi_string                      instanceName ) {

    /* Set global function pointer */
    global_callback = (omsi_callback_functions*) callback_functions;

    /* Log function call */
    filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2Instantiate: Allocates memory for sim_data");

    omsu->sim_data = (sim_data_t*)global_callback->allocateMemory(1, sizeof(sim_data_t));
    if (!omsu->sim_data) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: In omsu_allocate_sim_data: Not enough memory.");
        return omsi_error;
    }

    omsu->sim_data->model_vars_and_params = (omsi_values*)global_callback->allocateMemory(1, sizeof(omsi_values));
    if (!omsu->sim_data->model_vars_and_params) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: In omsu_allocate_sim_data: Not enough memory.");
        return omsi_error;
    }

    omsu->sim_data->pre_vars = (omsi_values*)global_callback->allocateMemory(1, sizeof(omsi_values));
    if (!omsu->sim_data->pre_vars) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: In omsu_allocate_sim_data: Not enough memory.");
        return omsi_error;
    }


    omsu->sim_data->initialization = NULL;
    /*omsi_data->sim_data->initialization = (omsi_function_t*) global_callback->allocateMemory(1, sizeof(omsi_function_t));
    if (!omsi_data->sim_data->initialization) {
        return omsi_error;
    }*/

    omsu->sim_data->simulation = (omsi_function_t*) global_callback->allocateMemory(1, sizeof(omsi_function_t));
    if (!omsu->sim_data->simulation) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                        "fmi2Instantiate: In omsu_allocate_sim_data: Not enough memory.");
        return omsi_error;
    }

    if (omsu_instantiate_omsi_function_func_vars(omsu->sim_data->simulation, omsu->sim_data->model_vars_and_params) != omsi_ok) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: in omsu_allocate_sim_data: Could not instantiate omsi_function variables.");
        return omsi_error;
    }


    omsu->sim_data->zerocrossings_vars = NULL;
    /*omsi_data->sim_data->zerocrossings_vars = (omsi_bool *) global_callback->allocateMemory(omsi_data->model_data->n_zerocrossings, sizeof(omsi_bool));*/
    omsu->sim_data->pre_zerocrossings_vars = NULL;
    /*omsi_data->sim_data->pre_zerocrossings_vars = (omsi_bool *) global_callback->allocateMemory(omsi_data->model_data->n_zerocrossings, sizeof(omsi_bool));*/

    /* ToDo: Add error cases */
    return omsi_ok;
}


/*
 * Instantiate omsi_function_t function_vars..
 */
omsi_status omsu_instantiate_omsi_function_func_vars (omsi_function_t*    omsi_function,
                                                      omsi_values*        function_vars) {

    /* Allocate memory */


    /* Set function_vars */
    if (function_vars==NULL) {  /* allocate memory for own copy function_vars */
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: Dedicated memory for OMSI function variables not implemented.");
        return omsi_error;
    }
    else {  /* share function_vars with sim_data->global model_vars_and_params */
        omsi_function->function_vars = function_vars;
    }

    return omsi_ok;
}

/*
 * Allocates memory for omsi_function_t struct without inner algebraic system.
 * Called from generated code.
 */
omsi_function_t* omsu_instantiate_omsi_function (omsi_values* function_vars) {

    omsi_function_t* function;

    function = (omsi_function_t*) global_callback->allocateMemory(1, sizeof(omsi_function_t));
    if (!function) {
        /* Log Error */
        return NULL;
    }

    function->algebraic_system_t = NULL;

    omsu_instantiate_omsi_function_func_vars(function, function_vars);

    return function;
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


omsi_status instantiate_input_inner_output_indices (omsi_function_t*    omsi_function,
                                                    omsi_unsigned_int   n_input_vars,
                                                    omsi_unsigned_int   n_output_vars) {

    /* Check omsi_function */
    if (!omsi_function) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: Memory for omsi_function not allocated.");
        return omsi_error;
    }

    omsi_function->input_vars_indices = (omsi_index_type*) global_callback->allocateMemory(n_input_vars, sizeof(omsi_index_type));
    /* CHECK_MEMORY_ERROR(omsi_function->input_vars_indices) */

    omsi_function->output_vars_indices = (omsi_index_type*) global_callback->allocateMemory(n_output_vars, sizeof(omsi_index_type));
    /* CHECK_MEMORY_ERROR(omsi_function->output_vars_indices) */

    return omsi_ok;
}

