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
 *! \file omc_jacobian.h
 *
 */

#ifndef _OMC_JACOCOBIAN_H_
#define _OMC_JACOCOBIAN_H_

#include "omc_matrix.h"

typedef struct {
 int index;           /* index of ANALYTICAL_JACOBIAN Structure: data->simulationInfo->analyticJacobians */
 int (*columnCall)(void*, threadData_t*, ANALYTIC_JACOBIAN*, ANALYTIC_JACOBIAN*);
 omc_matrix* matrix;  /* matrix data */
 ANALYTIC_JACOBIAN* parentJacobian; /* is only used in the algebraic loop of the jacobian calculation */
} omc_jacobian;

omc_jacobian* create_omc_jacobian(int index,
                                  int (*columnCall)(void*, threadData_t*, ANALYTIC_JACOBIAN*, ANALYTIC_JACOBIAN*), ANALYTIC_JACOBIAN* parentJacobian,
                                  unsigned int size_rows, unsigned int size_cols, int nnz, omc_matrix_orientation orientation, omc_matrix_type type);

/*Analytic Jacobian*/
int get_analytic_jacobian(DATA* data, threadData_t* threadData, omc_jacobian* jac);
/*Numerical Jacobian -> Call may by flag; not used yet!*/
int function_residual(DATA* data, threadData_t *threadData, double *dx);
int get_numeric_jacobian(DATA* data, threadData_t *threadData, omc_jacobian* jac);

void free_omc_jacobian(omc_jacobian* jac);

#endif
