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

/*! \file nonlinear_solver.c
 */

#include "Lapack.h"

extern int dgesv_(int *n, int *nrhs, double *a, int *lda,
                  int *ipiv, double *b, int *ldb, int *info);

/*! \fn allocate memory for linear system solver lapack
 *
 */
int allocateLapackData(int size, void** voiddata)
{
  DATA_LAPACK* data = (DATA_LAPACK*) malloc(sizeof(DATA_LAPACK));

  data->ipiv = (int*) malloc(size*sizeof(int));
  //assertStreamPrint(NULL, 0 != data->ipiv, "Could not allocate data for linear solver lapack.");
  data->nrhs = 1;
  data->info = 0;
  data->work = _omsi_allocateVectorData(size);

  data->x = _omsi_allocateVectorData(size);
  data->b = _omsi_allocateVectorData(size);
  data->A = _omsi_allocateMatrixData(size, size);

  *voiddata = (void*)data;
  return 0;
}

/*! \fn free memory of lapack
 *
 */
int freeLapackData(void **voiddata)
{
  DATA_LAPACK* data = (DATA_LAPACK*) *voiddata;

  free(data->ipiv);
  _omsi_deallocateVectorData(data->work);

  _omsi_deallocateVectorData(data->x);
  _omsi_deallocateVectorData(data->b);
  _omsi_deallocateMatrixData(data->A);

  return 0;
}

/*! \fn solve linear system with lapack method
 *
 *  \param [in]  [data]
 *               [sysNumber] index of the corresponding linear system
 *
 *  \author wbraun
 */
int solveLapack(DATA_LAPACK* lapackData, omsi_t *omsiData, omsi_linear_system_t *linearSystem)
{
  int iflag = 1;
  int success = 0;

  /* We are given the number of the linear system.
   * We want to look it up among all equations. */
  int eqSystemNumber = linearSystem->equation_index;
  int indexes[2] = {1,eqSystemNumber};
  omsi_scalar_t residualNorm = 0;
  double tmpJacEvalTime;

  //infoStreamPrintWithEquationIndexes(LOG_LS, 0, indexes, "Start solving Linear System %d (size %d) at time %g with Lapack Solver", eqSystemNumber, (int) linearSystem->n_system, omsiData->sim_data->time_value);

  //rt_ext_tp_tick(&(lapackData->timeClock));

  /* update vector x*/
  linearSystem->get_x(&omsiData->sim_data, lapackData->x);

  /* update matrix A */
  linearSystem->get_a_matrix(&omsiData->sim_data, lapackData->A);

  /* update vector b (rhs) */
  linearSystem->get_b_vector(&omsiData->sim_data, lapackData->b);

  //tmpJacEvalTime = rt_ext_tp_tock(&(lapackData->timeClock));
  //infoStreamPrint(LOG_LS_V, 0, "###  %f  time to set Matrix A and vector b.", tmpJacEvalTime);

  /* Log A*x=b 
  if(ACTIVE_STREAM(LOG_LS_V)){
    _omsi_printVector(lapackData->x, "Vector old x", LOG_LS_V);
    _omsi_printMatrix(lapackData->A, "Matrix A", LOG_LS_V);
    _omsi_printVector(lapackData->b, "Vector b", LOG_LS_V);
  }
  */

  //rt_ext_tp_tick(&(lapackData->timeClock));

  /* Solve system */
  dgesv_((int*) &linearSystem->n_system,
         (int*) &lapackData->nrhs,
		     lapackData->A->data,
         (int*) &linearSystem->n_system,
		     lapackData->ipiv,
		     lapackData->b->data,
         (int*) &linearSystem->n_system,
         &lapackData->info);

  //infoStreamPrint(LOG_LS_V, 0, "Solve System: %f", rt_ext_tp_tock(&(lapackData->timeClock)));
  if(lapackData->info < 0)
  {
    //warningStreamPrint(LOG_LS, 0, "Error solving linear system of equations (no. %d) at time %f. Argument %d illegal.", (int)linearSystem->equation_index, omsiData->sim_data->time_value, (int)lapackData->info);
    success = lapackData->info;
  }
  else if(lapackData->info > 0)
  {
    //warningStreamPrint(LOG_LS, 0,
    //    "Failed to solve linear system of equations (no. %d) at time %f, system is singular for U[%d, %d].",
    //    (int)linearSystem->equation_index, omsiData->sim_data->time_value, (int)lapackData->info+1, (int)lapackData->info+1);

    success = lapackData->info;

    /* debug output 
    if (ACTIVE_STREAM(LOG_LS)){
      _omsi_printMatrix(lapackData->A, "Matrix U", LOG_LS);

      _omsi_printVector(lapackData->b, "Output vector x", LOG_LS);
    }
    */
  }

  if (0 == success){
	/* take the solution */
	lapackData->x = _omsi_addVectorVector(lapackData->x, lapackData->x, lapackData->b); // x = xold(work) + xnew(b)

	/* update inner equations */
	linearSystem->eval_residual(&omsiData->sim_data, lapackData->x, lapackData->work, iflag);
	residualNorm = _omsi_euclideanVectorNorm(lapackData->work);

	if ((isnan(residualNorm)) || (residualNorm>1e-4)){
	/*
	warningStreamPrint(LOG_LS, 0,
		"Failed to solve linear system of equations (no. %d) at time %f. Residual norm is %.15g.",
		(int)linearSystem->equation_index, omsiData->sim_data->time_value, residualNorm);
	*/
	printf("Residual norm to high %f or not a number\n", residualNorm);
	success = -1;
	}
	else
	{
	  linearSystem->set_x(&omsiData->sim_data, lapackData->x);
	}

    /*
    if (ACTIVE_STREAM(LOG_LS_V)){
      infoStreamPrint(LOG_LS_V, 1, "Residual Norm %.15g of solution x:", residualNorm);
      infoStreamPrint(LOG_LS_V, 0, "System %d numVars %d.", eqSystemNumber, modelInfoGetEquation(&data->modelData->modelDataXml,eqSystemNumber).numVar);

      for(i = 0; i < systemData->size; ++i) {
        infoStreamPrint(LOG_LS_V, 0, "[%d] %s = %.15g", i+1, modelInfoGetEquation(&data->modelData->modelDataXml,eqSystemNumber).vars[i], systemData->x[i]);
      }

      messageClose(LOG_LS_V);
    }
    */
  }

  return success;
}
