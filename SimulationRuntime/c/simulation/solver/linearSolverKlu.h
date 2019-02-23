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

/*! \file linearSolverKlu.h
 */

#include "omc_config.h"

#ifdef WITH_UMFPACK
#ifndef _LINEARSOLVERKLU_H_
#define _LINEARSOLVERKLU_H_


#include "simulation_data.h"
#include "suitesparse/Include/amd.h"
#include "suitesparse/Include/klu.h"
#include "omc_jacobian.h"
#include "linearSystem.h"
#include "omc_math.h"
#include "omc_jacobian.h"


typedef struct DATA_KLU
{
  omc_jacobian *jacobian;
  klu_symbolic *symbolic;
  klu_numeric *numeric;
  klu_common common;

  _omc_vector* work;
  _omc_vector* b;
  _omc_vector* x;

  rtclock_t timeClock;             /* time clock */
  int numberSolving;

} DATA_KLU;

int allocateKluData(int index,
                    int (*columnCall)(void*, threadData_t*, ANALYTIC_JACOBIAN*, ANALYTIC_JACOBIAN*), ANALYTIC_JACOBIAN* parentJacobian,
                    unsigned int size_rows, unsigned int size_cols, int nnz, omc_matrix_orientation orientation, omc_matrix_type type, void **data);
int freeKluData(void **data);
int solveKlu(DATA *data, threadData_t *threadData, LINEAR_SYSTEM_DATA* systemData, double* aux_x);

#endif
#endif
