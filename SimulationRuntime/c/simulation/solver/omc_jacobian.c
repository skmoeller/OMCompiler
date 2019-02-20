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
 *! \file omc_jacobian.c
 */

#include "omc_config.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "simulation_data.h"
#include "simulation/simulation_info_json.h"
#include "simulation/solver/external_input.h"
#include "util/omc_error.h"
#include "util/varinfo.h"
#include "model_help.h"

#include "omc_jacobian.h"
#include "omc_matrix.h"

omc_jacobian* create_omc_jacobian(int index,
    int (*columnCall)(void*, threadData_t*, ANALYTIC_JACOBIAN*, ANALYTIC_JACOBIAN*), ANALYTIC_JACOBIAN* parentJacobian,
    unsigned int size_rows, unsigned int size_cols, int nnz, omc_matrix_orientation orientation, omc_matrix_type type)
{
  omc_jacobian* jac = (omc_jacobian*) malloc(sizeof(omc_jacobian));
  jac->index = index;
  jac->columnCall = columnCall;
  jac->parentJacobian = parentJacobian;
  jac->matrix = allocate_matrix(size_rows, size_cols, nnz, orientation, type);

  return (jac);
}

int get_analytic_jacobian(DATA* data, threadData_t* threadData, omc_jacobian* jac)
{
    int i,ii,j,k,l;

    ANALYTIC_JACOBIAN* jacobian = &(data->simulationInfo->analyticJacobians[jac->index]);

    int nth = 0;
    int nnz = jacobian->sparsePattern.numberOfNoneZeros;

    for(i=0; i < jacobian->sizeRows; i++)
    {
      jacobian->seedVars[i] = 1;

      (jac->columnCall)(data, threadData, jacobian, jac->parentJacobian);

      for(j = 0; j < jacobian->sizeCols; j++)
      {
        if(jacobian->seedVars[j] == 1)
        {
          ii = jacobian->sparsePattern.leadindex[j];
          while(ii < jacobian->sparsePattern.leadindex[j+1])
          {
            l  = jacobian->sparsePattern.index[ii];
            set_matrix_element(jac->matrix, i, l, nth, jacobian->resultVars[l]);
            nth++;
            ii++;
          };
        }
      }
      /* de-activate seed variable for the corresponding color */
      jacobian->seedVars[i] = 0;
    }

    return 0;
}

int get_numeric_jacobian(DATA* data, threadData_t *threadData, omc_jacobian* jac)
{
  /*
  const double delta_h = sqrt(DBL_EPSILON*2e1);
  double delta_hh;
  double xsave;

  double* x;

  int i,j,k,val;
  int nth = 0;

  int size_A = data->modelData->nStates;
  int size_z = data->modelData->nVariablesReal - 2*data->modelData->nStates;

  double* x0 = (double*)calloc(size_A,sizeof(double));
  double* x1 = (double*)calloc(size_A,sizeof(double));
  double *xScaling = (double*)calloc(size_A,sizeof(double));

  assertStreamPrint(threadData,0!=x0,"calloc failed");
  assertStreamPrint(threadData,0!=x1,"calloc failed");

  function_residual(data, threadData, x0);


  // solverData->f1 must be set outside this function based on x
  for(i = 0; i < size_A; i++) {
      xsave = x[i];
      delta_hh = delta_h * (fabs(xsave) + 1.0);
      if ((xsave + delta_hh >=  data->modelData->realVarsData[i].attribute.max))
          delta_hh *= -1;
      x[i] += delta_hh / xScaling[i];
      // Calculate scaled difference quotient
      delta_hh = 1. / delta_hh * xScaling[i];

      function_residual(data, threadData, x1);

      for(j = 0; j < size_A; j++) {
        val = (x1[j] - x0[j]) * delta_hh;
        set_matrix_element(jac->matrix, i, j, nth, val);
        nth++;
      }
      x[i] = xsave;
      }

  free(xScaling);
  free(x0);
  free(x1);
  */

  return 0;
}


void free_omc_jacobian(omc_jacobian* jac)
{
  free_matrix(jac->matrix);
  free(jac);
}
