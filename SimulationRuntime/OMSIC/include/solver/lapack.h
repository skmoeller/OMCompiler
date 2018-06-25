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

#include <math.h>
#include <stdlib.h>
//#include <string.h> /* memcpy */

#include "math/omsi_matrix.h"
#include "math/omsi_vector.h"
#include "math/omsi_math.h"
#include "omsi.h"
#include "omsi_eqns_system.h"
#include "util/rtclock.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Struct to store all informations for LAPACK dgesv
 */
typedef struct DATA_LAPACK {
    omsi_int        n;      /* number of linear equations*/
    omsi_int        nrhs;   /* number of right hand sides, default =1 */
    omsi_real*      A;      /* array of dimension (lda,n) */
    omsi_int        lda;    /* leading dimension of array A */
    omsi_int*       ipiv    /* array of dimension n, stores pivot indices for permutation matrix P */
    omsi_real*      B;      /* array of dimension (ldb, nrhs), right hand side of equation system on entry. On exit if info=0 solution (n x nrhs) Matrix X */
    omsi_int        ldb;    /* leading dimension of array B */
    omsi_int        info;   /* =0 if succesfull, <0 if Info=-i the i-th */
} DATA_LAPACK;

/* function prototypes */
extern int dgesv_(int *n, int *nrhs, double *a, int *lda, int *ipiv,
                  double *b, int *ldb, int *info);
int allocateLapackData(int size, DATA_LAPACK **data);
int freeLapackData(DATA_LAPACK *data);
int setLapackData(DATA_LAPACK *lapackData, sim_data_t *sim_data, int n);
int getLapackData(DATA_LAPACK *lapackData, sim_data_t *sim_data);
int solveLapack(DATA_LAPACK* lapackData, omsi_t *omsiData,
                omsi_linear_system_t *linearSystem);
int solveLapack_new(omsi_t *omsiData, omsi_vector_t *result_x);

#ifdef __cplusplus
}   /* end of extern "C" { */
#endif
#endif
