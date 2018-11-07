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
 * Solver specific functions for LAPACK solver.
 */

/** @addtogroup LAPACK_SOLVER LAPACK solver
 *  \ingroup LIN_SOLVER
  *  @{ */


#include <solver_lapack.h>

/*
 * ============================================================================
 * Allocaten, Initialization and freeing of solver_specific_data
 * ============================================================================
 */

/**
 * Allocates memory for LAPACK specific solver data and saves it in solver instance.
 *
 * \param [in,out]  general_solver_data     Solver instance.
 * \return          solver_status           solver_ok on success and
 *                                          solver_error on failure.
 */
solver_status allocate_lapack_data(solver_data* general_solver_data) {

    /* Variables */
    solver_data_lapack* lapack_data;

    /* check for correct solver */
    if (!solver_instance_correct(general_solver_data, solver_lapack, "allocate_lapack_data")) {
        return solver_error;
    }

    /* Check if general_solver_data has already specific data */
    if (general_solver_data->specific_data != NULL) {
        solver_logger("Solver-Error in function allocate_lapack_data: Pointer to solver specific data is not NULL.");
        general_solver_data->state = solver_error_state;
        return solver_error;
    }

    /* Allocate memory */
    lapack_data = (solver_data_lapack*) solver_allocateMemory(1, sizeof(solver_data_lapack));
    if (!lapack_data) {
        solver_logger("Solver-Error in function allocate_lapack_data: Can't allocate memory for lapack_data.");
        general_solver_data->specific_data = NULL;
        general_solver_data->state = solver_error_state;
        return solver_error;
    }

    lapack_data->A = (solver_real*) solver_allocateMemory(general_solver_data->dim_n*general_solver_data->dim_n, sizeof(solver_real));
    lapack_data->ipiv = (solver_int*) solver_allocateMemory(general_solver_data->dim_n, sizeof(solver_int));
    lapack_data->b = (solver_real*) solver_allocateMemory(general_solver_data->dim_n, sizeof(solver_real));
    if (!lapack_data->A || !lapack_data->ipiv || !lapack_data->b) {
        solver_logger("Solver-Error in function allocate_lapack_data: Can't allocate memory for lapack_data.");
        general_solver_data->specific_data = NULL;
        general_solver_data->state = solver_error_state;
        return solver_error;
    }

    /* Set specific data and model status*/
    general_solver_data->specific_data = lapack_data;
    general_solver_data->state = solver_instantiated;

    return solver_ok;
}


/**
 *  Frees LAPACK specific solver data.
 *
 * \param [in,out]  general_solver_data     Solver instance.
 * \return          solver_status           solver_ok on success and
 *                                          solver_error on failure.
 */
solver_status lapack_free_data(solver_data* general_solver_data) {

    /* Variables */
    solver_data_lapack* lapack_data;

    /* check for correct solver */
    if (!solver_instance_correct(general_solver_data, solver_lapack, "lapack_free_data")) {
        return solver_error;
    }

    lapack_data = general_solver_data->specific_data;

    solver_freeMemory(lapack_data->A);
    solver_freeMemory(lapack_data->ipiv);
    solver_freeMemory(lapack_data->b);

    solver_freeMemory(lapack_data);

    return solver_ok;
}



/**
 * Set dimension `dim_n` of matrix `A` in LAPACK specific solver data.
 *
 * \param [in,out]  general_solver_data     Solver instance.
 * \return          solver_status           solver_ok on success and
 *                                          solver_error on failure.
 */
solver_status set_dim_lapack_data(solver_data* general_solver_data) {

    /* Variables */
    solver_data_lapack* lapack_data;

    /* check for correct solver */
    if (!solver_instance_correct(general_solver_data, solver_lapack, "set_dim_lapack_data")) {
        return solver_error;
    }

    lapack_data = general_solver_data->specific_data;

    /* Set dimensions for matrices and arrays */
    lapack_data->n = general_solver_data->dim_n;
    lapack_data->nrhs = 1;
    lapack_data->lda = general_solver_data->dim_n;
    lapack_data->ldb = general_solver_data->dim_n;

    return solver_ok;
}


/*
 * ============================================================================
 * Getters and Setters
 * ============================================================================
 */

/**
 * Get value of element `A(row, column)`.
 *
 * \param [in]      solver_specififc_data   LAPACK specific solver data.
 * \param [in]      row                     Row index.
 * \param [in]      column                  Column index.
 * \param [in,out]  value                   On input: Pointer to previously allocated memory. <br>
 *                                          On output: Value of `A(row, column)` written in `*value`.
 */
void solver_lapack_get_A_element(void*                  solver_specififc_data,
                                 solver_unsigned_int    row,
                                 solver_unsigned_int    column,
                                 solver_real*           value) {

    /* Variables */
    solver_data_lapack*    lapack_data;

    lapack_data = solver_specififc_data;

    /* Access A(row,column) */
    *value = lapack_data->A[row+column*lapack_data->lda];

}


/**
 * Set value of element `A(row, column)` in LAPACK solver specific data.
 *
 * \param [in,out]  specific_data   LAPACK specific solver data. Value of
 *                                  `A(row,column)` gets overwritten with `*value`.
 * \param [in]      row             Row index.
 * \param [in]      column          Column index.
 * \param [in]      value           Pointer to value for`A(row,column)`.
 */
void solver_lapack_set_A_element(void*                  specific_data,
                                 solver_unsigned_int    row,
                                 solver_unsigned_int    column,
                                 solver_real*           value) {

    /* Variables */
    solver_data_lapack* lapack_data;

    lapack_data = specific_data;

    /* Access A(row,column) */
    lapack_data->A[row+column*lapack_data->lda] = *value;
}


/**
 * Get value of element `b(index)` in LAPACK solver specific data.
 *
 * \param [in]      specific_data   LAPACK specific solver data. Value of
 *                                  `b(index)` gets overwritten with `*value`.
 * \param [in]      index           Index.
 * \param [in, out] value           On input: Pointer to previously allocated memory. <br>
 *                                  On output: Value of `b(index)` written in `*value`.
 */
void solver_lapack_get_b_element(void*                  specific_data,
                                 solver_unsigned_int    index,
                                 solver_real*           value) {
    /* Variables */
    solver_data_lapack* lapack_data;

    lapack_data = specific_data;

    /* Access b(index) */
    *value = lapack_data->b[index];
}


/**
 * Set value of element `b(index)` in LAPACK solver specific data.
 *
 * \param [in,out] specific_data    LAPACK specific solver data. Value of
 *                                  `b(index)` gets overwritten with `*value`.
 * \param [in] index                Index.
 * \param [in] value                Pointer to value for `b(index)`.
 */
void solver_lapack_set_b_element(void*                  specific_data,
                                 solver_unsigned_int    index,
                                 solver_real*           value) {

    /* Variables */
    solver_data_lapack* lapack_data;

    lapack_data = specific_data;

    /* Access b(index) */
    lapack_data->b[index] = *value;
}


/*
 * ============================================================================
 * Solve call
 * ============================================================================
 */

/**
 * Call LAPACK DGESV to solve linear equation system.
 *
 * \param [in,out]  specific_data   LAPACK specific solver data.
 *                                  Gets overwritten by DGESV call.
 *
 * \return          Returns `solver_status` `solver_okay` if solved successful,
 *                  otherwise `solver_error`.
 */
solver_state solver_lapack_solve(void* specific_data) {

    /* Variables */
    solver_data_lapack* lapack_data;

    lapack_data = specific_data;

    /* solve equation system */
    dgesv_(&lapack_data->n,
           &lapack_data->nrhs,
            lapack_data->A,
           &lapack_data->lda,
            lapack_data->ipiv,
            lapack_data->b,
           &lapack_data->ldb,
           &lapack_data->info);

    if (lapack_data->info < 0) {
        /* ToDO: Log */
        solver_logger("Solver-Error in function solver_lapack_solve: the %d-th argument had an illegal value.", (-1)*lapack_data->info);
        return solver_error;
    }
    else if (lapack_data->info > 0) {
        /* ToDo: Log */
        solver_logger("Solver-Error in function solver_lapack_solve:  U(%d,%d) is exactly zero.\n "
                "The factorization has been completed, but the factor U is exactly"
                "singular, so the solution could not be computed", lapack_data->info, lapack_data->info);
        return solver_error;
    }

    return solver_ok;
}


/*
 * ============================================================================
 * Print and debug functions
 * ============================================================================
 */

/**
 * Debug function to print all informations in LAPACK specific solver data.
 *
 * @param [in] general_solver_data  Solver instance.
 */
void solver_print_lapack_data(solver_data* general_solver_data) {

    /* ToDo: Not overflow protected yet!*/
    /* Variables */
    solver_data_lapack* lapack_data;
    solver_char buffer[MAX_BUFFER_SIZE];
    solver_int i;

    lapack_data = general_solver_data->specific_data;

    /* check for correct solver */
    if (!solver_instance_correct(general_solver_data, solver_lapack, "print_lapack_data")) {
        return;
    }

    sprintf(buffer, "Number of linear equations: %i\n", lapack_data->n);
    sprintf(buffer, "Number of right hand sides: %i\n", lapack_data->nrhs);

    sprintf(buffer, "A in row major order:\n\t| ");
    for (i=0; i<lapack_data->lda*lapack_data->n; i++) {
        sprintf(buffer, "\t| %f\t\n", lapack_data->A[i]);
    }
    sprintf(buffer, "\n");

    sprintf(buffer, "Leading dimension of A: %i\n", lapack_data->lda);
    sprintf(buffer, "Pivot indices:");
    for (i=0; i<lapack_data->n; i++) {
        sprintf(buffer, " %i", lapack_data->ipiv[i]);
    }
    sprintf(buffer, "\n");

    sprintf(buffer, "b in row major order:\n\t| ");
    for (i=0; i<lapack_data->ldb*lapack_data->nrhs; i++) {
        sprintf(buffer, "\t| %f\t\n", lapack_data->b[i]);
    }
    sprintf(buffer, "\n");

    sprintf(buffer, "Leading dimension of b: %i\n", lapack_data->ldb);
    sprintf(buffer, "Info:%i\n", lapack_data->info);

    solver_logger(buffer);
}





/** @} */
