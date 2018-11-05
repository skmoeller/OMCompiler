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

#include <solver_lapack.h>


/*
 * Get an equation system A*x=b in omsi_algebraic_system_t format.
 * Compute the solution using LAPACK DGESV.
 * If a correct solution is found omsi_ok is returned and equationSystemFunc
 * was updated.
 * If omsi_warning is returned the found solution is not correct. Then
 * equationSysteFunc is not updated.
 * If omsi_error is returned a critical error occurred.
 */
omsi_status solveLapack(omsi_algebraic_system_t*    linearSystem,
                        const omsi_values*          read_only_vars_and_params,
                        omsi_callback_functions*    callback_functions){

    /* variables */
    solver_data_lapack* lapack_data;
    omsi_status status;

    /* set global functions */
    if (callback_functions != NULL) {
        global_callback = callback_functions;
    }

    /* allocate memory and copy informations into lapack_data */
    lapack_data = set_lapack_data((const omsi_algebraic_system_t*) linearSystem, read_only_vars_and_params);
    if (lapack_data == NULL) {
        return omsi_error;
    }


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
        printf("ERROR solving linear system: the %d-th argument had an illegal value\n", (-1)*lapack_data->info);
        lapack_free_data(lapack_data);
        return omsi_error;
    }
    else if (lapack_data->info > 0) {
        /* ToDo: Log */
        printf("ERROR solving linear system:  U(%d,%d) is exactly zero.\n", lapack_data->info, lapack_data->info);
        printf("The factorization has been completed, but the factor U is exactly"
                "singular, so the solution could not be computed\n");
        lapack_free_data(lapack_data);
        return omsi_error;
    }

    /* check if solution is correct */
    status = eval_residual(lapack_data, linearSystem, read_only_vars_and_params);
    if (status == omsi_ok) {
        /* copy solution into equationSystemFunc */

    }
    else {
        /* solution is not accuracte enough.
         * ToDo: Still proceed with this solution or or reject solution?
         */
    }

    /* free memory */
    lapack_free_data(lapack_data);

    return status;
}


/*
 * Copies necessary informations from equationSystemFunc into a new created
 * lapack_data structure.
 * Input:   equationSystemFunc
 * Output:  pointer to lapack_data, if pointer=NULL an error occurred
 */
solver_data_lapack* set_lapack_data(const omsi_algebraic_system_t* linear_system,
                             const omsi_values*             read_only_vars_and_params) {

    solver_data_lapack* lapack_data = (solver_data_lapack*) global_callback->allocateMemory(1, sizeof(solver_data_lapack));
    if (!lapack_data) {
        /* ToDo: log error out of memory */
        return NULL;
    }

    lapack_data->n = linear_system->functions->n_output_vars;
    lapack_data->nrhs = 1;
    lapack_data->lda = lapack_data->n;
    lapack_data->ldb = lapack_data->n;

    /* allocate memory */
    lapack_data->A = (omsi_real*) global_callback->allocateMemory(lapack_data->lda*lapack_data->n, sizeof(omsi_real));
    lapack_data->ipiv = (omsi_int*) global_callback->allocateMemory(lapack_data->n, sizeof(omsi_int));
    lapack_data->b = (omsi_real*) global_callback->allocateMemory(lapack_data->ldb*lapack_data->nrhs, sizeof(omsi_real));
    if (!lapack_data->A || !lapack_data->ipiv || !lapack_data->b) {
        /* ToDo: log error out of memory */
        return NULL;
    }

    set_lapack_a(lapack_data, linear_system);
    set_lapack_b(lapack_data, linear_system, read_only_vars_and_params);

    return lapack_data;
}

/*
 * Read data from equationSystemFunc and
 * set matrix A in row-major order.
 */
omsi_status set_lapack_a (solver_data_lapack*                      lapack_data,
                          const omsi_algebraic_system_t*    linear_system) {

    omsi_int i,j;

    for (i=0; i<lapack_data->lda; i++) {
        for (j=0; j<lapack_data->n; j++) {
            /* copy data from column-major to row-major style */
            lapack_data->A[i+j*lapack_data->lda] = linear_system->functions->function_vars->reals[i*lapack_data->n+j];
            /* ToDo: where exactly is this data stored in function ???? */
        }
    }

    return omsi_ok;
}

/*
 * Sets right hand side b of equation system.
 * Evaluates residual function with iteration variables set to 0 to get b.
 */
omsi_status set_lapack_b (solver_data_lapack*                      lapack_data,
                          const omsi_algebraic_system_t*    linearSystem,
                          const omsi_values*                read_only_vars_and_params) {

    omsi_int i;

    /* set iteration vars to zero */
    if (!omsu_set_omsi_value(linearSystem->functions->function_vars,
                             &(linearSystem->functions->output_vars_indices),
                             linearSystem->n_iteration_vars,
                             0)) {
        return omsi_error;
    }

    /* evaluate residual function A*0-b=0 to get -b */
    linearSystem->functions->evaluate(linearSystem->functions, read_only_vars_and_params, lapack_data->b);

    /* flip sign */
    for (i=0; i<lapack_data->ldb; i++) {
        lapack_data->b[i] = -lapack_data->b[i];
    }

    return omsi_ok;
}


/*
 * Evaluate residual A*x-b=res and return omsi_ok if it is zero,
 * otherwise omsi_warning.
 */
omsi_status eval_residual(solver_data_lapack*              lapack_data,
                          omsi_algebraic_system_t*  linearSystem,
                          const omsi_values*        read_only_vars_and_params) {
    /* local variables */
    omsi_int increment = 1;

    omsi_real* res;        /* pointer for residuum vector */
    omsi_real dotProduct;

    /* allocate memory */
    res = (omsi_real*) global_callback->allocateMemory(lapack_data->n, sizeof(omsi_real));

    /* compute residuum A*x-b using generated function and save result in residuum */
    /* ToDo: function call */
    linearSystem->functions->evaluate(linearSystem->functions, read_only_vars_and_params, res);

    /* compute dot product <residuum, residuum> */
    dotProduct = ddot_(&lapack_data->n, res, &increment, res, &increment);

    /* free memory */
    global_callback->freeMemory(res);

    if (dotProduct < 1e-4) {  /* ToDo: use some accuracy */
        return omsi_ok;
    }
    else {
        /* ToDo: log Solution is not accurate enough */
        return omsi_warning;
    }
}


/*
 * Writes solution generated by solver call into omsi_function_t->function_vars
 * Input:   lapack_data
 * Output:  equationSystemFunc
 */
void get_result(omsi_function_t*    equationSystemFunc,
                solver_data_lapack*        lapack_data) {

    omsi_unsigned_int i;
    omsi_unsigned_int index;

    for (i=0; i<equationSystemFunc->n_output_vars;i++) {
        index = equationSystemFunc->output_vars_indices[i].index;
        equationSystemFunc->function_vars->reals[index] = lapack_data->b[i];
    }
}





/*
 * Debug function. Print all information of DATA_LAPACK structure.
 */
void printLapackData(solver_data_lapack*   lapack_data,
                     omsi_string    indent) {

    omsi_int i;

    printf("%snumber of linear equations: %i\n", indent, lapack_data->n);
    printf("%snumber of right hand sides: %i\n", indent, lapack_data->nrhs);

    printf("%sA in row major order:\n%s| ", indent, indent);
    for (i=0; i<lapack_data->lda*lapack_data->n; i++) {
        printf("%s%f\t\n", indent, lapack_data->A[i]);
    }
    printf("\n");

    printf("%sleading dimension of A: %i\n", indent, lapack_data->lda);
    printf("%spivot indices:", indent);
    for (i=0; i<lapack_data->n; i++) {
        printf(" %i", lapack_data->ipiv[i]);
    }
    printf("\n");

    printf("%sb in row major order:\n%s| ", indent, indent);
    for (i=0; i<lapack_data->ldb*lapack_data->nrhs; i++) {
        printf("%s%f\t\n", indent, lapack_data->b[i]);
    }
    printf("\n");

    printf("%sleading dimension of b: %i\n", indent, lapack_data->ldb);
    printf("%sinfo:%i\n", indent, lapack_data->info);
}





/*
 *
 * Solve equation systems A*x=b with
 *   A matrix of dimension (lda, n), row major order
 *   b vector of dimension (ldb, nrhs), row major order
 *
 */


/*
 * ====================================================================
 */


/*
 * Allocate memory for specific solver data and set dimensions in newly
 * allocated struct.
 */
solver_data_lapack* lapack_allocate (solver_unsigned_int n,
                                     solver_unsigned_int nrhs,
                                     solver_unsigned_int lda,
                                     solver_unsigned_int ldb) {

    /* Variables */
    solver_data_lapack* data;

    /* allocate memory */
    data = (solver_data_lapack*) solver_allocateMemory(1, sizeof(solver_data_lapack));

    data->A = (solver_real*) solver_allocateMemroy(lda*n, sizeof(solver_real));
    data->ipiv = (solver_int*) solver_allocateMemory(n, sizeof(solver_int));
    data->b = (solver_real*) solver_allocateMemory(ldb*nrhs, sizeof(solver_real));

    /* set dimensions */
    data->n = n;
    data->nrhs = nrhs;
    data->lda = lda;
    data->ldb = ldb;

    return data;
}


/*
 *  Frees lapack_data
 */
void lapack_free_data(solver_data_lapack* lapack_data) {
    solver_freeMemory(lapack_data->A);
    solver_freeMemory(lapack_data->ipiv);
    solver_freeMemory(lapack_data->b);

    solver_freeMemory(lapack_data);
}






