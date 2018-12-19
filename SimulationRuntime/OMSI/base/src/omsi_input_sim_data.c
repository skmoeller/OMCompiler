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

/** \file omsi_input_sim_data.c
 */

/** \defgroup initSimData Set SimData
 * \ingroup Initialization
 *
 * \brief Set `sim_data_t` struct with functions from generated code.
 *
 * Allocate memory for omsi_data->sim_data. Set up variables and parameters,
 * create data for initialization and simulation problem and all containg
 * algebraic systems.
 */

/** \addtogroup initSimData
  *  \{ */


#include <omsi_input_sim_data.h>

#define UNUSED(x) (void)(x)     /* ToDo: delete later */


/**
 * \brief Allocates memory and initializes sim_data_t struct with functions from generated code.
 *
 * Assumes memory is already allocated for omsi_data->sim_data.
 *
 * \param omsi_data             Pointer to OMSU data
 * \param template_function     Pointer to struct of callback functions in generated code.
 * \param callback_functions    Callback functions to be used from OMSI functions, e.g for
 *                              memory management or logging.
 * \return                      `omsi_status omsi_ok` if successful <br>
 *                              `omsi_status omsi_error` if something went wrong.
 */
omsi_status omsu_setup_sim_data(omsi_t*                             omsi_data,
                                omsi_template_callback_functions_t* template_function,
                                const omsi_callback_functions*      callback_functions) {

    /* set global function pointer */
    global_callback = (omsi_callback_functions*) callback_functions;
    solver_init_callbacks (global_callback->allocateMemory,
                           global_callback->freeMemory,
                           wrapper_alg_system_logger);

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



    return omsi_ok;
}


/**
 * \brief Set up omsi_function_t struct for initialization or simulation problem.
 *
 * \param sim_data
 * \param function_name
 * \param template_instantiate_function
 * \param function_vars
 * \return
 */
omsi_status omsu_setup_sim_data_omsi_function(sim_data_t*                   sim_data,
                                              omsi_string                   function_name,
                                              omsu_initialize_omsi_function template_instantiate_function,
                                              omsi_values*                  function_vars) {

    /* Variables */
    omsi_function_t* omsi_function;

    /* Set initialization or simulation problem */
    if (strcmp(function_name, "initialization")==0) {
        omsi_function = sim_data->initialization;
    }
    else if (strcmp(function_name, "simulation")==0) {
        omsi_function = sim_data->simulation;
    }
    else {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: Error while instantiating initialization problem with generated code.");
        return omsi_error;
    }

    /* Call generated initialization function for initialization problem */
    if (template_instantiate_function(omsi_function)==omsi_error) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: Error while instantiating initialization problem with generated code.");
        return omsi_error;
    }

    /* Set function variables. Either local copy or pointer to global model_vars_and_params*/
    if (omsu_instantiate_omsi_function_func_vars(omsi_function, function_vars)==omsi_error) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: Error while instantiating function variables of sim_data->simulation.");
        return omsi_error;
    }

    /* Set default solvers for omsi_function */
    if (omsu_set_default_solvers(omsi_function, function_name)) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: Could not instantiate default solvers for algebraic loops in %s problem.",
                function_name);
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

    UNUSED(instanceName);

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

    /* Allocate memory for initialization and simulation problem */
    omsu->sim_data->initialization = (omsi_function_t*) global_callback->allocateMemory(1, sizeof(omsi_function_t));
    if (!omsu->sim_data->initialization) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                    "fmi2Instantiate: In omsu_allocate_sim_data: Not enough memory.");
        return omsi_error;
    }

    omsu->sim_data->simulation = (omsi_function_t*) global_callback->allocateMemory(1, sizeof(omsi_function_t));
    if (!omsu->sim_data->simulation) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                    "fmi2Instantiate: In omsu_allocate_sim_data: Not enough memory.");
        return omsi_error;
    }

    /* Instantiate function_vars in all omsi_functions */
    if (omsu_instantiate_omsi_function_func_vars(omsu->sim_data->simulation, omsu->sim_data->model_vars_and_params) != omsi_ok) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: in omsu_allocate_sim_data: Could not instantiate omsi_function variables.");
        return omsi_error;
    }

    /* Allocate memory for zero crossings and set pointers in omsi_functions */
    omsu->sim_data->zerocrossings_vars = (omsi_real *) global_callback->allocateMemory(omsu->model_data->n_zerocrossings, sizeof(omsi_real));
    omsu->sim_data->pre_zerocrossings_vars = (omsi_real *) global_callback->allocateMemory(omsu->model_data->n_zerocrossings, sizeof(omsi_real));
    if (!omsu->sim_data->zerocrossings_vars || !omsu->sim_data->pre_zerocrossings_vars) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: in omsu_allocate_sim_data: Not enough memory.");
        return omsi_error;
    }
    omsu_set_zerocrossings_omsi_functions(omsu->sim_data->initialization,
            omsu->sim_data->zerocrossings_vars, omsu->sim_data->pre_zerocrossings_vars);
    omsu_set_zerocrossings_omsi_functions(omsu->sim_data->simulation,
                omsu->sim_data->zerocrossings_vars, omsu->sim_data->pre_zerocrossings_vars);

    /* ToDo: Add error cases */
    return omsi_ok;
}


/*
 * Instantiate omsi_function_t function_vars.
 */
omsi_status omsu_instantiate_omsi_function_func_vars (omsi_function_t*    omsi_function,
                                                      omsi_values*        function_vars) {

    /* Variables */
    omsi_unsigned_int i;

    /* Set function_vars */
    if (function_vars==NULL) {
        omsi_function->function_vars = NULL;
    }
    else {  /* share function_vars with sim_data->global model_vars_and_params */
        omsi_function->function_vars = function_vars;
        /* Set function_vars recursive on all sub omsi_functions. */
        for(i=0; i<omsi_function->n_algebraic_system; i++) {
            omsu_instantiate_omsi_function_func_vars(omsi_function->algebraic_system_t[i].jacobian, function_vars);
            omsu_instantiate_omsi_function_func_vars(omsi_function->algebraic_system_t[i].functions, function_vars);
        }
    }

    return omsi_ok;
}


omsi_status omsu_set_zerocrossings_omsi_functions (omsi_function_t* omsi_function,
                                                   omsi_real*       pointer_to_zerocrossings_vars,
                                                   omsi_real*       pointer_to_pre_zerocrossings_vars)
{
    /* Variables */
    omsi_unsigned_int i;

    if (omsi_function==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: Error in function omsu_set_zerocrossings_omsi_functions.");
        return omsi_error;
    }

    omsi_function->zerocrossings_vars = pointer_to_zerocrossings_vars;
    omsi_function->pre_zerocrossings_vars = pointer_to_pre_zerocrossings_vars;

    /* ToDo: Do for all alg systesm recursevly */
    for (i=0; i<omsi_function->n_algebraic_system; i++){
        omsu_set_zerocrossings_omsi_functions (omsi_function->algebraic_system_t[i].functions,
                pointer_to_zerocrossings_vars,
                pointer_to_pre_zerocrossings_vars);
        omsu_set_zerocrossings_omsi_functions (omsi_function->algebraic_system_t[i].jacobian,
                pointer_to_zerocrossings_vars,
                pointer_to_pre_zerocrossings_vars);
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
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
            "fmi2Instantiate: Could not allocate memory for omsi_function_t struct.");
        return NULL;
    }

    function->algebraic_system_t = NULL;
    function->local_vars = NULL;

    omsu_instantiate_omsi_function_func_vars(function, function_vars);

    return function;
}


/*
 * Allocates memory for omsi_algebraic_system_t struct.
 * Since n_conditions is unknown memory for zerocrossing_indices is not allocated!
 */
omsi_algebraic_system_t* omsu_instantiate_alg_system_array (omsi_unsigned_int n_algebraic_system) {

    /* Variables */
    omsi_algebraic_system_t* algebraic_system;

    if (n_algebraic_system==0) {
        return NULL;
    }

    /* allocate memory */
    algebraic_system = (omsi_algebraic_system_t*) global_callback->allocateMemory(n_algebraic_system, sizeof(omsi_algebraic_system_t));

    if (!algebraic_system) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
            "fmi2Instantiate: Could not allocate memory for omsi_algebraic_system_t struct.");
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

    if (n_reals>0) {
        values->reals = alignedMalloc(sizeof(omsi_real) * n_reals, 64);     /* ToDo: Switch 64 to some OS depending macro */
        if (!values->reals) {
            /* ToDo: log Error */
            return NULL;
        }
    }
    else {
        values->reals = NULL;
    }

    if (n_ints>0) {
        values->ints = alignedMalloc(sizeof(omsi_int) * n_reals, 64);
        if (!values->ints) {
            /* ToDo: log Error */
            return NULL;
        }
    }
    else {
        values->ints = NULL;
    }

    if (n_bools>0) {
        values->bools = alignedMalloc(sizeof(omsi_bool) * n_reals, 64);
        if (!values->bools) {
            /* ToDo: log Error */
            return NULL;
        }
    }
    else {
        values->bools = NULL;
    }

    values->externs = NULL;

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


omsi_status omsu_set_default_solvers (omsi_function_t*  omsi_function,
                                      omsi_string       omsi_function_name) {

    /* Variables */
    omsi_unsigned_int i, dim_n;
    omsi_status status;

    status = omsi_ok;

    if (omsi_function==NULL) {
        return omsi_ok;
    }

    /* Log function call */
    if (omsi_function->n_algebraic_system > 0) {
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2Instantiate: Set default solver for algebraic systems in omsi_function %s.",
            omsi_function_name);
    }

    for(i=0; i<omsi_function->n_algebraic_system; i++) {
        dim_n = omsi_function->algebraic_system_t[i].jacobian->n_output_vars;       /* Dimension of jacobian */

        /* Check if solver_data still unallocated */
        if (omsi_function->algebraic_system_t[i].solver_data!=NULL) {
            filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: Memory for solver_data in algebraic loop %i already allocated.",
                i);
            return omsi_error;
        }

        if (omsi_function->algebraic_system_t[i].isLinear) {
            /* Set default linear solver */
            omsi_function->algebraic_system_t[i].solver_data = solver_allocate(solver_lapack, dim_n);
        }
        else {
            /* Set default non-linear solver */
            omsi_function->algebraic_system_t[i].solver_data = solver_allocate(solver_kinsol, dim_n);
        }

        if (!omsi_function->algebraic_system_t[i].isLinear) {
            omsu_set_initial_guess(&omsi_function->algebraic_system_t[i]);
            solver_prepare_specific_data(omsi_function->algebraic_system_t[i].solver_data,
                    omsi_residual_wrapper,
                    &omsi_function->algebraic_system_t[i]);
        }
        else {
            solver_prepare_specific_data(omsi_function->algebraic_system_t[i].solver_data,
                    NULL,
                    NULL);
        }

        /* recursive call for all */
        status = omsu_set_default_solvers (omsi_function->algebraic_system_t[i].jacobian, "jacobian");
        if (status != omsi_ok) {
            return status;
        }

        status = omsu_set_default_solvers (omsi_function->algebraic_system_t[i].functions, "residual");
        if (status != omsi_ok) {
            return status;
        }
    }

    return status;
}

void omsu_set_initial_guess (omsi_algebraic_system_t* algebraic_system)
{
    /* Variables */
    omsi_real* initial_guess;
    omsi_unsigned_int i, index;

    /* Allocate memory */
    initial_guess = (omsi_real*) global_callback->allocateMemory(algebraic_system->solver_data->dim_n, sizeof(omsi_real));

    /* Read start values for initial guess */
    for (i=0; i<algebraic_system->solver_data->dim_n; i++) {
        index = algebraic_system->functions->output_vars_indices[i].index;
        initial_guess[i] = algebraic_system->functions->function_vars->reals[index];
    }

    /* Set initial guess in solver data */
    solver_set_start_vector(algebraic_system->solver_data, initial_guess);
}
