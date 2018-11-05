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

/*! \file */

/** @addtogroup SOLVER  OMSI Solver Library
  * \ingroup OMSI
  *  @{ */

#include <omsi_solver.h>
#include <solver_lapack.h>

#ifdef __cplusplus
extern "C" {
#endif



/* Set callback functions */
void solver_init_callbacks (solver_callback_allocate_memory allocateMemoryFunction,
                           solver_callback_free_memory     freeMemoryFunction) {

    /* set global callback functions */
    solver_allocateMemory = allocateMemoryFunction;
    solver_freeMemory = freeMemoryFunction;
}

#if 0
/*
 * ============================================================================
 * Memory management
 * ============================================================================
 */

/* Allocate function */
solver_data* solver_allocate(solver_name            name,
                             solver_unsigned_int    dim_n) {

    /* Variables */
    solver_data* solver;

    /* allocate memory */
    solver = (solver_data*) solver_allocateMemory(1, sizeof(solver_data));

    switch (name) {
        case solver_lapack:
            solver->specific_data = lapack_allocate(dim_n, 1, dim_n, dim_n);
        break;
        default:
    }

    /* set dimension */
    solver->dim_n = dim_n;

    /* set callback functions */
    switch (name) {
        case solver_lapack:
            solver->set_A_element = NULL;
            solver->set_F_func = NULL;
        break;
        default:
    }

    return solver;

}


/* Free function */
void solver_free(solver_data* solver) {

    /* free solver specific data */
    switch (solver->name) {
        case solver_lapack:
            lapack_free_data(solver->specific_data);
        break;
        default:
    }

    solver_freeMemory(solver);



}

#endif

/*
 * ============================================================================
 * Getters and setters
 * ============================================================================
 */


/** \fn
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
 */
// Ai[N+1] = {0, 3, 6, ...}
// Ap[NNZ] = {1, 2, 4, 1, 2, 4, ...}
// Ax[NNZ] = {0.1, 0.3, 0.5, 0.2, 0.4, 0.6, ...}
/*
*         / 0   0.2   0  \
*    A =  | 0.1  0    0  |
*         \ 0   0.4  0.6 /
*         / 0.2   0  \
*   A~ =  | 0.1   0  |
*         \ 0.4  0.6 /
*/

void set_matrix_A(const solver_data*            solver,     /* Detailed description */
                  const solver_unsigned_int*    column,
                  const solver_unsigned_int     n_column,
                  const solver_unsigned_int*    row,
                  const solver_unsigned_int     n_row,
                  solver_real**           value) {        /* value is array of dimension n_column*n_row
                                                         * and stores n_column-times-n_row matrix in row-major oder */

    if (!solver->linear) {
        /* ToDo: log error, no matrix A in non-linear case */
        return;
    }

    /* Variables */
    solver_unsigned_int i, j;

    if (column==NULL && row==NULL) {
        /* copy values element wise */
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                solver->set_A_element(i, j, value[i*solver->dim_n+j]);
            }
        }
    }
    else if (column==NULL && row != NULL) {
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                solver->set_A_element(i, row[j], value[i*solver->dim_n+j]);
            }
        }
    }
    else if (column!=NULL && row == NULL) {
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                solver->set_A_element(column[i], j, value[i*solver->dim_n+j]);
            }
        }
    }
    else {
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                solver->set_A_element(column[i], row[j], value[i*solver->dim_n+j]);
            }
        }
    }


}


/** \fn void get_matrix_A(solver_data*          solver,
                          solver_unsigned_int*  column,
                          solver_unsigned_int   n_column,
                          solver_unsigned_int*  row,
                          solver_unsigned_int   n_row,
                          solver_real*          value)
 *  \brief Reads matrix A and saves result in array value.
 *
 *  Used for linear solvers, to get values of matrix A stored in its solver specific data.
 */
void get_matrix_A(solver_data*          solver,     /**< [in] Struct with used solver, containing matrix A in solver specific format.
                                                      *       Has to be a linear solver. */
                  solver_unsigned_int*  column,     /**< [in] Array of dimension `n_column` of unsigned integers,
                                                      *       specifying which columns of matrix A to get.
                                                      *       If column equals `NULL`, get the first `n_column` columns of A. */
                  solver_unsigned_int   n_column,   /**< [in] Size of array `column`. Must be greater then 0 and less
                                                      *       or equal to number of columns of matrix A. */
                  solver_unsigned_int*  row,        /**< [in] Array of dimension `n_row` of unsigned integers,
                                                      *       specifying which rows of matrix A to get.
                                                      *       If rows equals `NULL`, get the first `n_row` rows of A. */
                  solver_unsigned_int   n_row,      /**< [in] Size of array `row`. Must be greater then 0 and less
                                                      *       or equal to number of rows of matrix A. */
                  solver_real**         value)      /**< [in/out] On input: Pointer to allocated memory of size `sizeof(solver_real)*n_column*n_row`.
                                                      *           On Output: Pointer to array containing specified columns and rows of matrix A in row-major-order. */
{
    /* Variables */
    solver_unsigned_int i, j;

    if (column==NULL && row==NULL) {
        /* copy values element wise */
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                solver->get_A_element(i, j, value[i*solver->dim_n+j]);
            }
        }
    }
    else if (column==NULL && row != NULL) {
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                solver->get_A_element(i, row[j], value[i*solver->dim_n+j]);
            }
        }
    }
    else if (column!=NULL && row == NULL) {
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                solver->get_A_element(column[i], j, value[i*solver->dim_n+j]);
            }
        }
    }
    else {
        for (i=0; i<n_column; i++) {
            for (j=0; j<n_row; j++) {
                solver->get_A_element(column[i], row[j], value[i*solver->dim_n+j]);
            }
        }
    }
}

/*
 * ============================================================================
 * Print and debug functions
 * ============================================================================
 */


#ifdef __cplusplus
}  /* end of extern "C" { */
#endif


/** @} */
