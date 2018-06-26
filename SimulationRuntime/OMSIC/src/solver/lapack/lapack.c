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

#include "solver/lapack.h"

/* forward global functions */
omsi_callback_allocate_memory   global_allocateMemory;
omsi_callback_free_memory       global_freeMemory;


/*
 * Get an equation system A*x=b in omsi_function_t format.
 * Compute the solution using LAPACK DGESV.
 * If a correct solution is found omsi_ok is returned and equationSystemFunc
 * was updated.
 * If omsi_warning is returned the found solution is not correct. Then
 * equationSysteFunc is not updated.
 * If omsi_error is returned a critical error occurred.
 */
omsi_status solveLapack(omsi_function_t* equationSystemFunc, omsi_callback_functions* callback_functions){

    /* variables */
    DATA_LAPACK* lapack_data;
    omsi_status status;

    /* set global functions */
    global_allocateMemory = callback_functions->allocateMemory;
    global_freeMemory = callback_functions->freeMemory;

    // allocate memory and copy informations into lapack_data
    lapack_data = set_lapack_data((const omsi_function_t*) equationSystemFunc);
    if (lapack_data == NULL) {
        return omsi_error;
    }


    /* solve equation system */
    dgesv_(&lapack_data->n,
           &lapack_data->nrhs,
            lapack_data->A,
           &lapack_data->lda,
            lapack_data->ipiv,
            lapack_data->B,
           &lapack_data->ldb,
           &lapack_data->info);

    if (lapack_data->info < 0) {
        // ToDO: Log
        printf("ERROR solving linear system: the %d-th argument had an illegal value\n", (-1)*lapack_data->info);
        freeLapackData(lapack_data);
        return omsi_warning;
    }
    else if (lapack_data->info > 0) {
        // ToDo: Log
        printf("ERROR solving linear system:  U(%d,%d) is exactly zero.\n", lapack_data->info, lapack_data->info);
        printf("The factorization has been completed, but the factor U is exactly"
                "singular, so the solution could not be computed\n");
        freeLapackData(lapack_data);
        return omsi_warning;
    }

    /* check if solution is correct */
    status = eval_residual(lapack_data);
    if (status == omsi_ok) {
        /* copy solution into equationSystemFunc */

    }

    /* free memory */
    freeLapackData(lapack_data);

    return omsi_ok;
}


/*
 * Copies necessary informations from equationSystemFunc into a new created
 * lapack_data structure.
 * Input:   equationSystemFunc
 * Output:  pointer to lapack_data, if pointer=NULL an error occurred
 */
DATA_LAPACK* set_lapack_data(const omsi_function_t* equationSystemFunc) {

    DATA_LAPACK* lapack_data = (DATA_LAPACK*) global_allocateMemory(1, sizeof(DATA_LAPACK));
    if (!lapack_data) {
        // ToDo: log error out of memory
        return NULL;
    }

    lapack_data->n = equationSystemFunc->n_output_vars;
    lapack_data->nrhs = 1;
    lapack_data->lda = lapack_data->n;
    lapack_data->ldb = lapack_data->n;

    /* allocate memory */
    lapack_data->A = (omsi_real*) global_allocateMemory(lapack_data->lda*lapack_data->n, sizeof(omsi_real));
    lapack_data->old_A = (omsi_real*) global_allocateMemory(lapack_data->lda*lapack_data->n, sizeof(omsi_real));
    lapack_data->ipiv = (omsi_int*) global_allocateMemory(lapack_data->n, sizeof(omsi_int));
    lapack_data->B = (omsi_real*) global_allocateMemory(lapack_data->ldb*lapack_data->nrhs, sizeof(omsi_real));
    lapack_data->old_B = (omsi_real*) global_allocateMemory(lapack_data->ldb*lapack_data->nrhs, sizeof(omsi_real));
    if (!lapack_data->A || !lapack_data->old_A || !lapack_data->ipiv || !lapack_data->B || !lapack_data->old_B) {
        // ToDo: log error out of memory
        return NULL;
    }

    set_lapack_a(lapack_data, equationSystemFunc);
    set_lapack_b(lapack_data, equationSystemFunc);

    return lapack_data;
}

/*
 * Read data from equationSystemFunc and
 * set matrix A in row-major order.
 */
void set_lapack_a (DATA_LAPACK* lapack_data, const omsi_function_t* equationSystemFunc) {

    omsi_int i,j;

    for (i=0; i<lapack_data->lda; i++) {
        for (j=0; j<lapack_data->n; j++) {
            /* copy data from column-major to row-major style */
            lapack_data->A[i+j*lapack_data->lda] = equationSystemFunc->function_vars->reals[i*lapack_data->n+j];
            lapack_data->old_A[i+j*lapack_data->lda] = equationSystemFunc->function_vars->reals[i*lapack_data->n+j];
            // ToDo: where exactly is this data stored in function_vars ????
        }
    }
}

/*
 * Read data from equationSystemFunc and
 * set matrix B in row-major order.
 */
void set_lapack_b (DATA_LAPACK* lapack_data, const omsi_function_t* equationSystemFunc) {

    omsi_int i,j;

    for (i=0; i<lapack_data->ldb; i++) {
        for (j=0; j<lapack_data->nrhs; j++) {
            /* copy data from column-major to row-major style */
            lapack_data->B[i+j*lapack_data->ldb] = equationSystemFunc->function_vars->reals[i*lapack_data->nrhs+j];
            lapack_data->old_B[i+j*lapack_data->ldb] = equationSystemFunc->function_vars->reals[i*lapack_data->nrhs+j];
            // ToDo: where exactly is this data stored in function_vars ????
        }
    }
}


/*
 * Evaluate residual A*x-b and return omsi_ok if it is zero, otherwise omsi_warning.
 * Overwrites lapack_data->old_B with residual.
 */
omsi_status eval_residual(DATA_LAPACK* lapack_data) {
    omsi_real alpha = 1;
    omsi_int increment = 1;
    omsi_real beta = -1;

    omsi_real dotProduct;

    /* compute residual old_A*x-old_B using BLAS */
    dgemv_("N",
            &lapack_data->n,
            &lapack_data->lda,
            &alpha,
            lapack_data->old_A,
            &lapack_data->lda,
            lapack_data->B,
            &increment,
            &beta,
            lapack_data->old_B,
            &increment);

    /* compute dot product <x,x> */
    dotProduct = ddot_(&lapack_data->n, lapack_data->old_B, &increment, lapack_data->old_B, &increment);

    if (dotProduct < 1e-4) {  // ToDo: use some accuracy
        return omsi_ok;
    }
    else {
        // ToDo: log Solution is not accurate enough
        return omsi_warning;
    }
}


/*
 * Writes solution generated by solver call into omsi_function_t->function_vars
 * Input:   lapack_data
 * Output:  equationSystemFunc
 */
void get_result(omsi_function_t*    equationSystemFunc,
                DATA_LAPACK*        lapack_data) {

    omsi_unsigned_int i;
    omsi_unsigned_int index;

    for (i=0; i<equationSystemFunc->n_output_vars;i++) {
        index = equationSystemFunc->output_vars_indices[i].index;
        equationSystemFunc->function_vars->reals[index] = lapack_data->B[i];
    }
}


/*
 *  Frees lapack_data
 */
void freeLapackData(DATA_LAPACK* lapack_data) {
    global_freeMemory(lapack_data->A);
    global_freeMemory(lapack_data->old_A);
    global_freeMemory(lapack_data->ipiv);
    global_freeMemory(lapack_data->B);
    global_freeMemory(lapack_data->old_B);

    global_freeMemory(lapack_data);
}
