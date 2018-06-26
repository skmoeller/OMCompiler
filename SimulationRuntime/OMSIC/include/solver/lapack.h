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

/*! \file linearSolverLapack.h
 */

#ifndef _LINEARSOLVERLAPACK_H_
#define _LINEARSOLVERLAPACK_H_

#include "omsi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Struct to store all informations for LAPACK calls
 */
typedef struct DATA_LAPACK {
    omsi_int        n;      /* number of linear equations*/
    omsi_int        nrhs;   /* number of right hand sides, default =1 */
    omsi_real*      A;      /* array of dimension (lda,n) containing matrix in
                             * row major order */
    omsi_int        lda;    /* leading dimension of array A */
    omsi_int*       ipiv;   /* array of dimension n, stores pivot indices for
                             * permutation matrix P */
    omsi_real*      B;      /* array of dimension (ldb, nrhs), containing right
                             * hand side of equation system in row major order on entry.
                             * On exit if info=0 solution (n x nrhs) Matrix X */
    omsi_int        ldb;    /* leading dimension of array B */
    omsi_int        info;   /* =0 if succesfull, <0 if Info=-i the i-th */

    omsi_real*      old_A;  /* contains copy of A for later usage */
    omsi_real*      old_B;  /* contains copy of B for later usage */
} DATA_LAPACK;


/* extern function prototypes */
extern omsi_int dgesv_(omsi_int *n, omsi_int *nrhs, omsi_real *a, omsi_int *lda,
                       omsi_int *ipiv, omsi_real *b, omsi_int *ldb, omsi_int *info);
extern omsi_int dgemv_(omsi_char* trans, omsi_int* m, omsi_int* n, omsi_real* alpha,
                       omsi_real* A, omsi_int* lda, omsi_real* x, omsi_int* incx,
                       omsi_real* beta, omsi_real* y, omsi_int* incy);
extern omsi_real ddot_(omsi_int* n, omsi_real* dx, omsi_int* incx, omsi_real* dy, omsi_int* incy);

/* function prototypes */
omsi_status solveLapack(omsi_function_t* equationSystemFunc, omsi_callback_functions* callback_functions);
DATA_LAPACK* set_lapack_data(const omsi_function_t* equationSystemFunc);
void set_lapack_a (DATA_LAPACK* lapack_data, const omsi_function_t* equationSystemFunc);
void set_lapack_b (DATA_LAPACK* lapack_data, const omsi_function_t* equationSystemFunc);
omsi_status eval_residual(DATA_LAPACK* lapack_data);
void get_result(omsi_function_t* equationSystemFunc, DATA_LAPACK* lapack_data);
void freeLapackData(DATA_LAPACK* lapack_data);

#ifdef __cplusplus
}   /* end of extern "C" { */
#endif
#endif
