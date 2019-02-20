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

/*! \file linearSolverKlu.c
 */

#include "omc_config.h"

#ifdef WITH_UMFPACK
#include <math.h>
#include <stdlib.h>
#include <string.h>


#include "simulation/options.h"
#include "simulation_data.h"
#include "simulation/simulation_info_json.h"
#include "util/omc_error.h"
#include "util/varinfo.h"
#include "model_help.h"

#include "linearSystem.h"
#include "linearSolverKlu.h"
#include "omc_math.h"
#include "omc_matrix.h"
#include "omc_sparse_matrix.h"

/*! \fn allocate memory for linear system solver Klu
 *
 */
int
allocateKluData(int index, int (*columnCall)(void* vptr, threadData_t* thread, struct ANALYTIC_JACOBIAN* jac, struct ANALYTIC_JACOBIAN* parjac), struct ANALYTIC_JACOBIAN* parentJacobian, unsigned int size_rows, unsigned int size_cols, int nnz, omc_matrix_orientation orientation, omc_matrix_type type, void **voiddata)
{
  DATA_KLU* data = (DATA_KLU*) malloc(sizeof(DATA_KLU));
  assertStreamPrint(NULL, 0 != data, "Could not allocate data for linear solver Klu.");

  data->jacobian = create_omc_jacobian(index, columnCall, parentJacobian, size_rows, size_cols, nnz, orientation, type);

  data->symbolic = NULL;
  data->numeric = NULL;
  data->work = _omc_allocateVectorData(size_rows);
  data->b = _omc_allocateVectorData(size_rows);
  data->x = _omc_createVector(size_rows, NULL);

  data->numberSolving = 0;
  klu_defaults(&(data->common));

  *voiddata = (void*)data;

  return 0;
}

/*! \fn free memory for linear system solver Klu
 *
 */
int
freeKluData(void **voiddata)
{
  TRACE_PUSH

  DATA_KLU* data = (DATA_KLU*) *voiddata;

  free_omc_jacobian(data->jacobian);
  _omc_destroyVector(data->x);
  _omc_deallocateVectorData(data->b);
  _omc_deallocateVectorData(data->work);

  if(data->symbolic)
    klu_free_symbolic(&data->symbolic, &data->common);
  if(data->numeric)
    klu_free_numeric(&data->numeric, &data->common);

  free(data);

  TRACE_POP
  return 0;
}

/*! \fn residual_wrapper for the residual function
 *
 */
static int residual_wrapper(_omc_vector* x, _omc_vector* f, void** data, LINEAR_SYSTEM_DATA* systemData)
{
  int iflag = 0;

  systemData->residualFunc(data, x->data, f->data, &iflag);
  return 0;
}

/*! \fn solve linear system with Klu method
 *
 *  \param  [in]  [data]
 *                [sysNumber] index of the corresponding linear system
 *
 *
 * author: wbraun
 */
int
solveKlu(DATA *data, threadData_t *threadData, LINEAR_SYSTEM_DATA* systemData, double* aux_x)
{
  void *dataAndThreadData[2] = {data, threadData};
  DATA_KLU* solverData = (DATA_KLU*)systemData->solverData[0];
  omc_sparse_matrix* matrixData = (omc_sparse_matrix*)solverData->jacobian->matrix->data;

  int i, j, status = 0, success = 0, n = systemData->size, eqSystemNumber = systemData->equationIndex, indexes[2] = {1,eqSystemNumber};
  double tmpJacEvalTime;
  int reuseMatrixJac = (data->simulationInfo->currentContext == CONTEXT_SYM_JACOBIAN && data->simulationInfo->currentJacobianEval > 0);

  _omc_setVectorData(solverData->x, aux_x);

  infoStreamPrintWithEquationIndexes(LOG_LS, 0, indexes, "Start solving Linear System %d (size %d) at time %g with Klu Solver",
  eqSystemNumber, (int) systemData->size,
  data->localData[0]->timeValue);
  rt_ext_tp_tick(&(solverData->timeClock));

  if (0 == systemData->method){
    if (!reuseMatrixJac){
      /* set A matrix */
      matrixData->ptr[0] = 0;
      systemData->setA(data, threadData, systemData);
      matrixData->ptr[matrixData->size_rows] = matrixData->nnz;
    }

    /* set b vector */
    systemData->setb(data, threadData, systemData);
  } else {
    if (!reuseMatrixJac){
      matrixData->ptr[0] = 0;
      /* calculate jacobian -> matrix A*/
      if (omc_flag[FLAG_LS_JACOBIAN]){
        get_numeric_jacobian(data, threadData, solverData->jacobian);
        infoStreamPrint(LOG_LS_V, 0, "jacobian uses numeric calculation\n");
      } else {
        if(systemData->jacobianIndex != -1){
          get_analytic_jacobian(data, threadData, solverData->jacobian);
          infoStreamPrint(LOG_LS_V, 0, "jacobian uses analytic calculation\n");
        } else {
          assertStreamPrint(threadData, 1, "jacobian function pointer is invalid" );
        }
      }
      matrixData->ptr[matrixData->size_rows] = matrixData->nnz;
    }
    /* calculate vector b (rhs) */
    _omc_copyVector(solverData->work, solverData->x);
    residual_wrapper(solverData->work, solverData->b, dataAndThreadData, systemData);
    _omc_negateVector(solverData->b);
  }
  tmpJacEvalTime = rt_ext_tp_tock(&(solverData->timeClock));
  systemData->jacobianTime += tmpJacEvalTime;
  infoStreamPrint(LOG_LS_V, 0, "###  %f  time to set Matrix A and vector b.", tmpJacEvalTime);

  if (ACTIVE_STREAM(LOG_LS_V))
  {
    infoStreamPrint(LOG_LS_V, 1, "Old solution x:");
    for(i = 0; i < matrixData->size_rows; ++i)
      infoStreamPrint(LOG_LS_V, 0, "[%d] %s = %g", i+1, modelInfoGetEquation(&data->modelData->modelDataXml,eqSystemNumber).vars[i], aux_x[i]);
    messageClose(LOG_LS_V);

    infoStreamPrint(LOG_LS_V, 1, "Matrix A n_rows = %d", matrixData->size_rows);
    for (i=0; i<matrixData->size_rows; i++){
      infoStreamPrint(LOG_LS_V, 0, "%d. Ap => %d -> %d", i, matrixData->ptr[i], matrixData->ptr[i+1]);
      for (j = matrixData->ptr[i]; j<matrixData->ptr[i+1]; j++){
        infoStreamPrint(LOG_LS_V, 0, "A[%d,%d] = %f", i, matrixData->index[j], matrixData->data[j]);
      }
    }
    messageClose(LOG_LS_V);

    for (i = 0; i<matrixData->size_rows; i++)
      infoStreamPrint(LOG_LS_V, 0, "b[%d] = %e", i, solverData->b->data[i]);
  }
  rt_ext_tp_tick(&(solverData->timeClock));


  /* symbolic pre-ordering of A to reduce fill-in of L and U */
  if (0 == solverData->numberSolving)
  {
    infoStreamPrint(LOG_LS_V, 0, "Perform analyze settings:\n - ordering used: %d\n - current status: %d", solverData->common.ordering, solverData->common.status);
    solverData->symbolic = klu_analyze(matrixData->size_cols, matrixData->ptr, matrixData->index, &solverData->common);
  }

  /* if reuseMatrixJac use also previous factorization */
  if (!reuseMatrixJac)
  {
    /* compute the LU factorization of A */
    if (0 == solverData->common.status){
      if(solverData->numeric){
        /* Just refactor using the same pivots, but check that the refactor is still accurate */
        klu_refactor(matrixData->ptr, matrixData->index, matrixData->data, solverData->symbolic, solverData->numeric, &solverData->common);
        klu_rgrowth(matrixData->ptr, matrixData->index, matrixData->data, solverData->symbolic, solverData->numeric, &solverData->common);
        infoStreamPrint(LOG_LS_V, 0, "Klu rgrowth after refactor: %f", solverData->common.rgrowth);
        /* If rgrowth is small then do a whole factorization with new pivots (What should this tolerance be?) */
        if (solverData->common.rgrowth < 1e-3){
          klu_free_numeric(&solverData->numeric, &solverData->common);
          solverData->numeric = klu_factor(matrixData->ptr, matrixData->index, matrixData->data, solverData->symbolic, &solverData->common);
          infoStreamPrint(LOG_LS_V, 0, "Klu new factorization performed.");
        }
      } else {
        solverData->numeric = klu_factor(matrixData->ptr, matrixData->index, matrixData->data, solverData->symbolic, &solverData->common);
      }
    }
  }

  if (0 == solverData->common.status){
    if (1 == systemData->method){
      if (klu_solve(solverData->symbolic, solverData->numeric, matrixData->size_cols, 1, solverData->b->data, &solverData->common)){
        success = 1;
      }
    } else {
      if (klu_tsolve(solverData->symbolic, solverData->numeric, matrixData->size_cols, 1, solverData->b->data, &solverData->common)){
        success = 1;
      }
    }
  }

  infoStreamPrint(LOG_LS_V, 0, "Solve System: %f", rt_ext_tp_tock(&(solverData->timeClock)));

  /* print solution */
  if (1 == success){

    if (1 == systemData->method){

      /* take the solution */
      solverData->x = _omc_addVectorVector(solverData->x, solverData->work, solverData->b); // x = xold(work) + xnew(b)

      /* update inner equations */
      residual_wrapper(solverData->x, solverData->work, dataAndThreadData, systemData);
    } else {
      /* the solution is automatically in x */
      memcpy(aux_x, systemData->b, sizeof(double)*systemData->size);
    }

    if (ACTIVE_STREAM(LOG_LS_V))
    {
      infoStreamPrint(LOG_LS_V, 1, "Solution x:");
      infoStreamPrint(LOG_LS_V, 0, "System %d numVars %d.", eqSystemNumber, modelInfoGetEquation(&data->modelData->modelDataXml,eqSystemNumber).numVar);

      for(i = 0; i < systemData->size; ++i)
        infoStreamPrint(LOG_LS_V, 0, "[%d] %s = %g", i+1, modelInfoGetEquation(&data->modelData->modelDataXml,eqSystemNumber).vars[i], aux_x[i]);

      messageClose(LOG_LS_V);
    }
  }
  else
  {
    warningStreamPrint(LOG_STDOUT, 0,
      "Failed to solve linear system of equations (no. %d) at time %f, system status %d.",
        (int)systemData->equationIndex, data->localData[0]->timeValue, status);
  }
  solverData->numberSolving += 1;

  return success;
}

#endif
