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

/*! \file nonlinearSolverNewton.h
 */

#ifndef _NEWTONITERATION_H_
#define _NEWTONITERATION_H_

#include "Newton.h"

#include <math.h>
#include <stdlib.h>
#include <string.h> /* memcpy */

//#include "external_input.h"

#include "../omsi_math/omsi_matrix.h"
#include "../omsi_math/omsi_vector.h"

#include "../omsi_math/omsi_math.h"
#include "../../../../OSI/omsi.h"
#include "../../../../OSI/omsi_eqns_system.h"
#include "../../../util/rtclock.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DATA_NEWTON
{
  int initialized; /* 1 = initialized, else = 0*/
  omsi_vector_t* resScaling;
  omsi_vector_t* fvecScaled;

  int newtonStrategy;

  int n;
  omsi_vector_t* x;
  omsi_vector_t* fvec;
  double xtol;
  double ftol;
  int nfev;
  int maxfev;
  int info;
  double epsfcn;
  omsi_vector_t* fjac;
  omsi_vector_t* rwork;
  int* iwork;
  int calculate_jacobian;
  int factorization;
  int numberOfIterations; /* over the whole simulation time */
  int numberOfFunctionEvaluations; /* over the whole simulation time */

  /* damped newton */
  omsi_vector_t* x_new;
  omsi_vector_t* x_increment;
  omsi_vector_t* f_old;
  omsi_vector_t* fvec_minimum;
  omsi_vector_t* delta_f;
  omsi_vector_t* delta_x_vec;

   rtclock_t timeClock;

} DATA_NEWTON;


int allocateNewtonData(int size, void** data);
int freeNewtonData(void** data);
int _omc_newton(int(*f)(int*, double*, double*, void*, int), DATA_NEWTON* solverData, void* userdata);

#ifdef __cplusplus
}
#endif

#endif
