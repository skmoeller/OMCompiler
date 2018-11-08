/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2014, Open Source Modelica Consortium (OSMC),
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

/** \file solver_api.c
 *
 * Application interface for OMSI solver.
 */

/** @addtogroup SOLVER OMSI Solver Library
  *  @{ */

#include <solver_api.h>

#include <solver_lapack.h>

#ifdef __cplusplus
extern "C" {
#endif



/**
 * \brief Set callback functions for memory management and logging.
 *
 * \param allocateMemoryFunction    Pointer to function for memory allocation.
 * \param freeMemoryFunction        Pointer to function for memory deallocation.
 * \param loggerFunction            Pointer to function for logging.
 */
void solver_init_callbacks (solver_callback_allocate_memory allocateMemoryFunction,
                            solver_callback_free_memory     freeMemoryFunction,
                            solver_callback_logger          loggerFunction) {

    /* set global callback functions */
    solver_allocateMemory = allocateMemoryFunction;
    solver_freeMemory = freeMemoryFunction;
    solver_logger = loggerFunction;
}


/*
 * ============================================================================
 * Memory management
 * ============================================================================
 */



/**
 * \brief Allocate memory for solver instance.
 *
 * \param [in]  name    Name of solver to use in this solver instance.
 * \param [in]  dim_n   Dimension `n` of square matrix ´A´.
 * \return              Returns newly allocated `solver_data* solver` instance.
 */
solver_data* solver_allocate(solver_name            name,
                             solver_unsigned_int    dim_n) {

    /* Variables */
    solver_data* solver;
    solver_linear_callbacks* lin_callbacks;

    /* allocate memory */
    solver = (solver_data*) solver_allocateMemory(1, sizeof(solver_data));

    switch (name) {
        case solver_lapack:
            solver->name = solver_lapack;
            solver->dim_n = dim_n;
            allocate_lapack_data(solver);

        break;
        default:
            solver_logger("Solver-Error in function solver_allocate: No valid solver_name given.");
            solver_freeMemory(solver);
            return NULL;
    }

    /* set dimension */
    solver->dim_n = dim_n;

    /* set callback functions */
    switch (name) {
        case solver_lapack:
            lin_callbacks = solver->solver_callbacks;

            lin_callbacks->get_A_element = &solver_lapack_get_A_element;
            lin_callbacks->set_A_element = &solver_lapack_set_A_element;

            lin_callbacks->get_b_element = &solver_lapack_get_b_element;
            lin_callbacks->set_b_element = &solver_lapack_set_b_element;

            lin_callbacks->solve_eq_system = &solver_lapack_solve;
        break;
        default:
            solver_logger("Solver-Error in function solver_allocate: No valid solver_name given.");
            solver_freeMemory(solver);
            return NULL;
    }

    /* Set solver state */
    solver->state = solver_initializated;

    return solver;

}


/** Frees memory of struct solver_data.
 *
 * \param [in,out] solver   Pointer to solver instance.
 */
void solver_free(solver_data* solver) {

    /* free solver specific data */
    switch (solver->name) {
        case solver_lapack:
            lapack_free_data(solver->specific_data);
        break;
        default:
            if (solver->specific_data != NULL) {
                solver_logger("Solver-Error in function solver_free: No solver"
                        "specified in solver_name, but solver->specific_data is not NULL");
            }
    }

    solver_freeMemory(solver);
}


/**
 * \brief Prepare specific solver data.
 *
 * E.g. set dimensions for matrices or functions.
 *
 * \param [in]  solver  Pointer to solver instance.
 * \return              Returns `solver_status` `solver_okay` if solved successful,
 *                      otherwise `solver_error`.
 */
solver_status prepare_specific_solver_data (solver_data* solver) {

    switch (solver->name) {
        case solver_lapack:
            return set_dim_lapack_data(solver);
        break;
        default:
            solver_logger("Solver-Error in function prepare_specific_solver_data: No solver"
                    "specified in solver_name.");
            return solver_error;
    }
}


/*
 * ============================================================================
 * Getters and setters
 * ============================================================================
 */

/** \fn void set_matrix_A(const solver_data*            solver,
 *                const solver_unsigned_int*    column,
 *                const solver_unsigned_int     n_column,
 *                const solver_unsigned_int*    row,
 *                const solver_unsigned_int     n_row,
 *                solver_real**           value)
 * \brief Sets matrix A with values from array value.
 *
 * Sets specified columns and rows of matrix A in solver specific data to
 * values from array value. If no columns and/or rows are specified (set to
 * NULL) all elements in those rows / columns are set to given values.
 *
 *   e.g set_matrix_A(solver, [1,2], 2, [2,3,5], 3, [0.1, 0.2, 0.3, 0.4, 0.5, 0.6]);
 *   will set n-times-n matrix A, for some n>= 5, to something like:
 *         / a_11  a_12  a_13  ... \
 *         | 0.1   0.2   a_23      |
 *         | 0.3   0.4   a_33      |
 *     A = | a_41  a_42  a_43      |
 *         | 0.5   0.6   a_53      |
 *         \ ...               ... /
 *
 * \param [in,out]  solver      Struct with used solver, containing matrix A in
 *                              solver specific format. Has to be a linear solver.
 * \param [in]      column      Array of dimension `n_column` of unsigned integers,
 *                              specifying which columns of matrix A to get. If
 *                              column equals `NULL`, get the first `n_column`
 *                              columns of A.
 * \param [in]      n_column    Size of array `column`. Must be greater then 0
 *                              and less or equal to number of columns of matrix A.
 * \param [in]      row         Array of dimension `n_row` of unsigned integers,
 *                              specifying which rows of matrix A to get. If rows
 *                              equals `NULL`, get the first `n_row` rows of A.
 * \param [in]      n_row       Size of array `row`. Must be greater then 0 and
 *                              less or equal to number of rows of matrix A.
 * \param [in]      value       Pointer to matrix with values, stored as array
 *                              in column-major order of size `n_column*n_row`.
 */
void set_matrix_A(const solver_data*            solver,
                  const solver_unsigned_int*    column,
                  const solver_unsigned_int     n_column,
                  const solver_unsigned_int*    row,
                  const solver_unsigned_int     n_row,
                  solver_real**                 value) {

    if (!solver->linear) {
        /* ToDo: log error, no matrix A in non-linear case */
        return;
    }

    /* Variables */
    solver_unsigned_int i, j;
    solver_linear_callbacks* lin_callbacks;

    lin_callbacks = solver->solver_callbacks;

    if (column==NULL && row==NULL) {
        /* copy values element wise */
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                lin_callbacks->set_A_element(solver->specific_data, i, j, value[i*solver->dim_n+j]);
            }
        }
    }
    else if (column==NULL && row != NULL) {
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                lin_callbacks->set_A_element(solver->specific_data, i, row[j], value[i*solver->dim_n+j]);
            }
        }
    }
    else if (column!=NULL && row == NULL) {
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                lin_callbacks->set_A_element(solver->specific_data, column[i], j, value[i*solver->dim_n+j]);
            }
        }
    }
    else {
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                lin_callbacks->set_A_element(solver->specific_data, column[i], row[j], value[i*solver->dim_n+j]);
            }
        }
    }
}


/** \fn void get_matrix_A(solver_data*          solver,
 *                        solver_unsigned_int*  column,
 *                        solver_unsigned_int   n_column,
 *                        solver_unsigned_int*  row,
 *                        solver_unsigned_int   n_row,
 *                        solver_real**         value)
 *
 *  Reads matrix A and saves result in array value.
 *  Used for linear solvers, to get values of matrix A stored in its solver
 *  specific data.
 *
 * \param [in]     solver       Struct with used solver, containing matrix A in
 *                              solver specific format. Has to be a linear solver.
 * \param [in]     column       Array of dimension `n_column` of unsigned integers,
 *                              specifying which columns of matrix A to get. If
 *                              column equals `NULL`, get the first `n_column`
 *                              columns of A.
 * \param [in]      n_column    Size of array `column`. Must be greater then 0
 *                              and less or equal to number of columns of matrix A.
 * \param [in]      row         Array of dimension `n_row` of unsigned integers,
 *                              specifying which rows of matrix A to get. If rows
 *                              equals `NULL`, get the first `n_row` rows of A.
 * \param [in]      n_row       Size of array `row`. Must be greater then 0 and
 *                              less or equal to number of rows of matrix A.
 * \param [in,out]  value       On input: Pointer to allocated memory of size
 *                              `sizeof(solver_real)*n_column*n_row`. <br>
 *                              On output: Pointer to array containing specified
 *                              columns and rows of matrix A in row-major-order.
 */
void get_matrix_A(solver_data*          solver,
                  solver_unsigned_int*  column,
                  solver_unsigned_int   n_column,
                  solver_unsigned_int*  row,
                  solver_unsigned_int   n_row,
                  solver_real**         value) {

    /* Variables */
    solver_unsigned_int i, j;
    solver_linear_callbacks* lin_callbacks;

    lin_callbacks = solver->solver_callbacks;

    if (column==NULL && row==NULL) {
        /* copy values element wise */
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                lin_callbacks->get_A_element(solver->specific_data, i, j, value[i*solver->dim_n+j]);
            }
        }
    }
    else if (column==NULL && row != NULL) {
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                lin_callbacks->get_A_element(solver->specific_data, i, row[j], value[i*solver->dim_n+j]);
            }
        }
    }
    else if (column!=NULL && row == NULL) {
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                lin_callbacks->get_A_element(solver->specific_data, column[i], j, value[i*solver->dim_n+j]);
            }
        }
    }
    else {
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                lin_callbacks->get_A_element(solver->specific_data, column[i], row[j], value[i*solver->dim_n+j]);
            }
        }
    }
}

/*
 * ============================================================================
 * Print and debug functions
 * ============================================================================
 */

/**
 * Print all data in solver_instance.
 *
 * \param [in] solver       Solver instance.
 */
void print_solver_data (solver_data* solver) {

    /* Variables */
    solver_char buffer[MAX_BUFFER_SIZE];
    solver_unsigned_int length;
    solver_linear_callbacks* lin_callbacks;

    length = 0;
    length = snprintf(buffer, MAX_BUFFER_SIZE-length,
            "Solver data print:\n");
    length = snprintf(buffer+length, MAX_BUFFER_SIZE-length,
            "name: \t %s\n", solver_name2string(solver->name));
    length = snprintf(buffer+length, MAX_BUFFER_SIZE-length,
            "linear: \t %s\n", solver->linear ? "solver_true":"solver_false");
    length = snprintf(buffer+length, MAX_BUFFER_SIZE-length,
            "info: \t %d\n", solver->info);
    length = snprintf(buffer+length, MAX_BUFFER_SIZE-length,
            "dim_n: \t %u\n", solver->dim_n);

    switch (solver->name) {
        case solver_lapack:
            solver_print_lapack_data(buffer, MAX_BUFFER_SIZE, &length, solver);
            break;
        default:
            length = snprintf(buffer+length, MAX_BUFFER_SIZE-length,
                    "No solver specific data.\n");
            break;
    }

    length = snprintf(buffer+length, MAX_BUFFER_SIZE-length,
            "solver_callbacks: \t %p\n", solver->solver_callbacks);
    switch (solver->linear) {
        case solver_true:
            lin_callbacks = solver->solver_callbacks;
            length = snprintf(buffer+length, MAX_BUFFER_SIZE-length,
                    "get_A_element set: \t %s\n", lin_callbacks->get_A_element?"yes":"no");
            length = snprintf(buffer+length, MAX_BUFFER_SIZE-length,
                    "set_A_element set: \t %s\n", lin_callbacks->set_A_element?"yes":"no");
            length = snprintf(buffer+length, MAX_BUFFER_SIZE-length,
                    "get_b_element set: \t %s\n", lin_callbacks->get_b_element?"yes":"no");
            length = snprintf(buffer+length, MAX_BUFFER_SIZE-length,
                    "set_b_element set: \t %s\n", lin_callbacks->set_b_element?"yes":"no");
            length = snprintf(buffer+length, MAX_BUFFER_SIZE-length,
                    "solve_eq_system set: \t %s\n", lin_callbacks->solve_eq_system?"yes":"no");
            break;
        default:

            break;
    }

    /* print buffer */
    solver_logger(buffer);
}


/**
 * \brief Returns solver name as string.
 *
 * \param [in] solver   Pointer to solver instance.
 * \return              String with solver name.
 */
solver_string solver_get_name (solver_data* solver) {

    return solver_name2string(solver->name);
}

/*
 * ============================================================================
 * Solve call
 * ============================================================================
 */

/**
 * \brief Calls solve function for registered solver.
 *
 * Checks if all necessary data is already set.
 *
 * \param solver    Solver instance.
 *
 * \return          Returns `solver_status` `solver_okay` if solved successful,
 *                  otherwise `solver_error`.
 */
solver_status solver_linear_solve(solver_data* solver) {

    /* Variables */
    solver_linear_callbacks* lin_callbacks;


    /* Check if solver is ready */
    if (!solver_func_call_allowed (solver, solver_ready, "solver_linear_solver")) {
        return solver_error;
    }

    lin_callbacks = solver->solver_callbacks;

    /* Call solve function */
    return lin_callbacks->solve_eq_system(solver->specific_data);
}

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif


/** @} */
