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
#include <string.h> /* memcpy */

#include "math/omsi_matrix.h"
#include "math/omsi_vector.h"
#include "math/omsi_math.h"
#include "omsi.h"
#include "omsi_eqns_system.h"
#include "util/rtclock.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DATA_LAPACK
{
  int *ipiv;  /* vector pivot values */
  int n;	  /* number of linear equations and unknowns */
  int nrhs;   /* number of righthand sides*/
  int info;   /* output */
  omsi_vector_t* work;

  omsi_vector_t* x;
  omsi_vector_t* b;
  omsi_matrix_t* A;
  rtclock_t timeClock;

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
