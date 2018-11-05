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

#include <omsi_solver.h>
#include <solver_helper.h>

#include <stdio.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Struct to store all informations for LAPACK calls
 */
typedef struct solver_data_lapack {
    solver_int      n;      /* number of linear equations*/
    solver_int      nrhs;   /* number of right hand sides, default =1 */
    solver_real*    A;      /* array of dimension (lda,n) containing matrix in
                             * row major order */
    solver_int      lda;    /* leading dimension of array A */
    solver_int*     ipiv;   /* array of dimension n, stores pivot indices for
                             * permutation matrix P */
    solver_real*    b;      /* array of dimension (ldb, nrhs), containing right
                             * hand side of equation system in row major order on entry.
                             * On exit if info=0 solution (n x nrhs) Matrix X */
    solver_int      ldb;    /* leading dimension of array B */
    solver_int      info;   /* =0 if succesfull, <0 if Info=-i the i-th */
} solver_data_lapack;


/* extern function prototypes */
extern solver_int dgesv_(solver_int *n, solver_int *nrhs, solver_real *a, solver_int *lda,
                       solver_int *ipiv, solver_real *b, solver_int *ldb, solver_int *info);
extern solver_real ddot_(solver_int* n, solver_real* dx, solver_int* incx, solver_real* dy, solver_int* incy);

/* function prototypes */
/*
omsi_status solveLapack(omsi_algebraic_system_t*            linearSystem,
                        const omsi_values*                  read_only_vars_and_params,
                        omsi_callback_functions*            callback_functions);

solver_data_lapack* set_lapack_data(const omsi_algebraic_system_t* linear_system,
                             const omsi_values*             read_only_vars_and_params);

omsi_status set_lapack_a (solver_data_lapack*                      lapack_data,
                          const omsi_algebraic_system_t*    linear_system);

omsi_status set_lapack_b (solver_data_lapack*                      lapack_data,
                          const omsi_algebraic_system_t*    linearSystem,
                          const omsi_values*                read_only_vars_and_params);

omsi_status eval_residual(solver_data_lapack*              lapack_data,
                          omsi_algebraic_system_t*  linearSystem,
                          const omsi_values*        read_only_vars_and_params);

void get_result(omsi_function_t*                            equationSystemFunc,
                solver_data_lapack*                                lapack_data);

void lapack_free_data(solver_data_lapack* lapack_data);

void printLapackData(solver_data_lapack*   lapack_data,
                     omsi_string    indent);
*/

#ifdef __cplusplus
}   /* end of extern "C" { */
#endif
#endif
