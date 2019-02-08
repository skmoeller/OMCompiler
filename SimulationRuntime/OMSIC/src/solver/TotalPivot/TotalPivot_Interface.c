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

#include "../../../include/solver/TotalPivot_Interface.h"


void debugMatrixDoubleLS(int logName, char* matrixName, double* matrix, int n, int m)
{
/*
  if(ACTIVE_STREAM(logName))
  {
    int i, j;
    int sparsity = 0;
    char *buffer = (char*)malloc(sizeof(char)*m*18);

    infoStreamPrint(logName, 1, "%s [%dx%d-dim]", matrixName, n, m);
    for(i=0; i<n;i++)
    {
      buffer[0] = 0;
      for(j=0; j<m; j++)
      {
        if (sparsity)
        {
          if (fabs(matrix[i + j*(m-1)])<1e-12)
            sprintf(buffer, "%s 0", buffer);
          else
            sprintf(buffer, "%s *", buffer);
        }
        else
        {
          sprintf(buffer, "%s%12.4g ", buffer, matrix[i + j*(m-1)]);
        }
      }
      infoStreamPrint(logName, 0, "%s", buffer);
    }
    messageClose(logName);
    free(buffer);
  }
  */
}

void debugVectorDoubleLS(int logName, char* vectorName, double* vector, int n)
{
	/*
   if(ACTIVE_STREAM(logName))
  {
    int i;
    char *buffer = (char*)malloc(sizeof(char)*n*22);

    infoStreamPrint(logName, 1, "%s [%d-dim]", vectorName, n);
    buffer[0] = 0;
    for(i=0; i<n;i++)
    {
      if (vector[i]<-1e+300)
        sprintf(buffer, "%s -INF ", buffer);
      else if (vector[i]>1e+300)
        sprintf(buffer, "%s +INF ", buffer);
      else
        sprintf(buffer, "%s%16.8g ", buffer, vector[i]);
    }
    infoStreamPrint(logName, 0, "%s", buffer);
    free(buffer);
    messageClose(logName);
  }
  */
}

void debugStringLS(int logName, char* message)
{
	/*
  if(ACTIVE_STREAM(logName))
  {
    infoStreamPrint(logName, 1, "%s", message);
    messageClose(logName);
  }
  */
}

void debugIntLS(int logName, char* message, int value)
{
	/*
  if(ACTIVE_STREAM(logName))
  {
    infoStreamPrint(logName, 1, "%s %d", message, value);
    messageClose(logName);
  }
  */
}

/*! \fn allocate memory for linear system solver totalpivot
 *
 *  \author bbachmann
 */
int allocateTotalPivotData(int size, void** voiddata)
{
  DATA_TOTALPIVOT* data = (DATA_TOTALPIVOT*) malloc(sizeof(DATA_TOTALPIVOT));

  /* memory for linear system */
  data->x = _omsi_allocateVectorData(size);
  data->x_tmp = _omsi_allocateVectorData(size);
  data->b = _omsi_allocateVectorData(size);
  data->Ab = _omsi_allocateMatrixData(size, size+1);

  /* used for pivot strategy */
  data->indRow =(int*) calloc(size,sizeof(int));
  data->indCol =(int*) calloc(size+1,sizeof(int));

  *voiddata = (void*)data;
  return 0;
}

/*! \fn free memory for nonlinear solver totalpivot
 *
 *  \author bbachmann
 */
int freeTotalPivotData(void** voiddata)
{
  DATA_TOTALPIVOT* data = (DATA_TOTALPIVOT*) *voiddata;

  /* memory for linear system */
  _omsi_deallocateVectorData(data->x);
  _omsi_deallocateVectorData(data->x_tmp);
  _omsi_deallocateVectorData(data->b);
  _omsi_deallocateMatrixData(data->Ab);

   /* used for pivot strategy */
  free(data->indRow);
  free(data->indCol);

  return 0;
}

/*! \fn wrapper_fvec_hybrd for the residual Function
 *      calls for the subroutine fcn(n, x, fvec, iflag, data)
 *
 *
 */
static int wrapper_fvec_totalpivot(double* x, double* f, void** data, int sysNumber)
{
  int currentSys = sysNumber;
  int iflag = 0;
  /* NONLINEAR_SYSTEM_DATA* systemData = &(((DATA*)data)->simulationInfo->nonlinearSystemData[currentSys]); */
  /* DATA_NEWTON* solverData = (DATA_NEWTON*)(systemData->solverData); */

  //(*((DATA*)data[0])->simulationInfo->linearSystemData[currentSys].residualFunc)(data, x, f, &iflag);
  return 0;
}

/*! \fn solve linear system with totalpivot method
 *
 *  \param [in]  [data]
 *                [sysNumber] index of the corresponing non-linear system
 *
 *  \author bbachmann
 */
int solveTotalPivot(DATA_TOTALPIVOT* totalPivotData, omsi_t *omsiData, omsi_linear_system_t *linearSystem)
{
  //void *dataAndThreadData[2] = {data, threadData};
  int i, j;
  int n = linearSystem->n_system, status;
  double fdeps = 1e-8;
  double xTol = 1e-8;
  int eqSystemNumber = linearSystem->equation_index;
  int indexes[2] = {1,eqSystemNumber};
  int rank;
  int iflag = 1;
  omsi_scalar_t residualNorm = 0;

  /* We are given the number of the linear system.
   * We want to look it up among all equations. */
  /* int eqSystemNumber = systemData->equationIndex; */
  int success = 0;
  double tmpJacEvalTime;

  //infoStreamPrintWithEquationIndexes(LOG_LS, 0, indexes, "Start solving Linear System %d (size %d) at time %g with Total Pivot Solver",
  //       eqSystemNumber, (int) linearSystem->n_system,
  //	 omsiData->sim_data->time_value);


  //TODO: debugVectorDoubleLS(LOG_LS_V,"SCALING",systemData->nominal,n);
  //TODO: debugVectorDoubleLS(LOG_LS_V,"Old VALUES",systemData->x,n);

  //rt_ext_tp_tick(&(totalPivotData->timeClock));

  /* reset matrix A */
  //vecConstLS(n*n, 0.0, systemData->A);

  /* update matrix A -> first n columns of matrix Ab*/
  linearSystem->get_a_matrix(&omsiData->sim_data, totalPivotData->Ab);
  //TODO: vecCopyLS(n*n, systemData->A, solverData->Ab);

  /* update vector b (rhs) -> -b is last column of matrix Ab*/
  //rt_ext_tp_tick(&(totalPivotData->timeClock));
  linearSystem->get_b_vector(&omsiData->sim_data, totalPivotData->b);
  _omsi_addMatrixColumn(totalPivotData->b, -1.0, totalPivotData->Ab, n*n);

  //tmpJacEvalTime = rt_ext_tp_tock(&(totalPivotData->timeClock));
  //infoStreamPrint(LOG_LS_V, 0, "###  %f  time to set Matrix A and vector b.", tmpJacEvalTime);
  //debugMatrixDoubleLS(LOG_LS_V,"LGS: matrix Ab",totalPivotData->Ab, n, n+1);

  //rt_ext_tp_tick(&(totalPivotData->timeClock));
  status = solveSystemWithTotalPivotSearchLS(n, totalPivotData->x_tmp->data, totalPivotData->Ab->data, totalPivotData->indRow, totalPivotData->indCol, &rank);
  //infoStreamPrint(LOG_LS_V, 0, "Solve System: %f", rt_ext_tp_tock(&(totalPivotData->timeClock)));

  if (status != 0)
  {
	//warningStreamPrint(LOG_STDOUT, 0, "Error solving linear system of equations (no. %d) at time %f.", (int)linearSystem->equation_index, omsiData->sim_data->time_value);
    success = -1;
  } else {
    //debugVectorDoubleLS(LOG_LS_V, "SOLUTION:", totalPivotData->x, n+1);

    /* add the solution to old solution vector*/
	linearSystem->get_x(&omsiData->sim_data, totalPivotData->x);
	_omsi_addVector(totalPivotData->x, totalPivotData->x_tmp); // xnew(b) = xold(work) + xnew(b)

    //wrapper_fvec_totalpivot(systemData->x, solverData->b, dataAndThreadData, sysNumber);
	linearSystem->eval_residual(&omsiData->sim_data, totalPivotData->x, totalPivotData->x_tmp, iflag);
	residualNorm = _omsi_euclideanVectorNorm(totalPivotData->x_tmp);

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
		linearSystem->set_x(&omsiData->sim_data, totalPivotData->x);
	}

    /*
    if (ACTIVE_STREAM(LOG_LS_V)){
      infoStreamPrint(LOG_LS_V, 1, "Solution x:");
      infoStreamPrint(LOG_LS_V, 0, "System %d numVars %d.", eqSystemNumber, modelInfoGetEquation(&data->modelData->modelDataXml,eqSystemNumber).numVar);
      for(i=0; i<systemData->size; ++i)
      {
        infoStreamPrint(LOG_LS_V, 0, "[%d] %s = %g", i+1, modelInfoGetEquation(&data->modelData->modelDataXml,eqSystemNumber).vars[i], systemData->x[i]);
      }
      messageClose(LOG_LS_V);
    }*/
  }
  return success;
}
