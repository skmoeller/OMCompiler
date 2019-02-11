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
#include "util/omc_error.h"
#include "util/varinfo.h"
#include "model_help.h"

#include "omc_jacobian.h"
#include "omc_matrix.h"

omc_jacobian* create_omc_jacobian(int index, int (*columnCall)(void*, threadData_t*, ANALYTIC_JACOBIAN*, ANALYTIC_JACOBIAN*),
                                  unsigned int size_rows, unsigned int size_cols, int nnz, omc_matrix_orientation orientation, omc_matrix_type type)
{
  omc_jacobian* jac = (omc_jacobian*) malloc(sizeof(omc_jacobian));
  jac->index = index;
  jac->columnCall = columnCall;
  jac->matrix = allocate_matrix(size_rows, size_cols, nnz, orientation, type);
}

int get_omc_jacobian(DATA* data, threadData_t* threadData, omc_jacobian*)
{
    int i,ii,j,k,l;
    LINEAR_SYSTEM_DATA* systemData = &(((DATA*)data)->simulationInfo->linearSystemData[sysNumber]);

    const int index = systemData->jacobianIndex;
    ANALYTIC_JACOBIAN* jacobian = &(data->simulationInfo->analyticJacobians[systemData->jacobianIndex]);
    ANALYTIC_JACOBIAN* parentJacobian = systemData->parentJacobian;

    int nth = 0;
    int nnz = jacobian->sparsePattern.numberOfNoneZeros;

    for(i=0; i < jacobian->sizeRows; i++)
    {
      jacobian->seedVars[i] = 1;

      ((systemData->analyticalJacobianColumn))(data, threadData, jacobian, parentJacobian);

      for(j = 0; j < jacobian->sizeCols; j++)
      {
        if(jacobian->seedVars[j] == 1)
        {
          ii = jacobian->sparsePattern.leadindex[j];
          while(ii < jacobian->sparsePattern.leadindex[j+1])
          {
            l  = jacobian->sparsePattern.index[ii];
            systemData->setAElement(i, l, -jacobian->resultVars[l], nth, (void*) systemData, threadData);
            nth++;
            ii++;
          };
        }
      };

      /* de-activate seed variable for the corresponding color */
      jacobian->seedVars[i] = 0;
    }

    return 0;

}

void free_omc_jacobian(omc_matrix* jac)
{
  free(jac->columnCall);
  free_matrix(jac->matrix);
  free(jac);
}
