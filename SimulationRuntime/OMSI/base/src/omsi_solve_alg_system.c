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

#include <omsi_solve_alg_system.h>


#if 0

omsi_status omsi_solve_algebraic_system (omsi_algebraic_system_t* alg_system) {

    /* Log solver call */
    filtered_base_logger(global_logCategories, log_all, omsi_ok,
        "fmi2Something: Solve %s system %u of size %u with solver %s.",
        alg_system->isLinear ? "linear":"nonlinear",
        alg_system->id,
        alg_system->n_iteration_vars,
        solver_get_name(alg_system->solver_data));


    /* Check if it is possible to reuse matrix A or vector b */


    /* Update solver specific data */
    omsi_get_analytical_jacobian(alg_system);


    /* Call solver */



    /* Save results */


    return omsi_ok;
}


omsi_status omsi_get_analytical_jacobian (omsi_algebraic_system_t* alg_system) {

    /* Variables */
    omsi_unsigned_int i, j;
    omsi_real* seed;

    /* set seed vars */
    seed = global_callback->allocateMemory(alg_system->n_iteration_vars, sizeof(omsi_real));




    for (i=0; i<alg_system->n_iteration_vars; i++) {    /* ToDo: Add coloring here */
        /* activate seed for current row or color */

        alg_system->jacobian->evaluate(alg_system->jacobian, seed, NULL);

    }

    return omsi_ok;
}
#endif


#if 0
int getAnalyticalJacobianNewton(DATA* data, threadData_t *threadData, double* jac, int sysNumber)
{
  int i,j,k,l,ii,currentSys = sysNumber;
  NONLINEAR_SYSTEM_DATA* systemData = &(((DATA*)data)->simulationInfo->nonlinearSystemData[currentSys]);
  DATA_NEWTON* solverData = (DATA_NEWTON*)(systemData->solverData);
  const int index = systemData->jacobianIndex;

  memset(jac, 0, (solverData->n)*(solverData->n)*sizeof(double));

  for(i=0; i < data->simulationInfo->analyticJacobians[index].sparsePattern.maxColors; i++)
  {
    /* activate seed variable for the corresponding color */
    for(ii=0; ii < data->simulationInfo->analyticJacobians[index].sizeCols; ii++)
      if(data->simulationInfo->analyticJacobians[index].sparsePattern.colorCols[ii]-1 == i)
        data->simulationInfo->analyticJacobians[index].seedVars[ii] = 1;

    systemData->analyticalJacobianColumn(data, threadData);

    for(j = 0; j < data->simulationInfo->analyticJacobians[index].sizeCols; j++)
    {
      if(data->simulationInfo->analyticJacobians[index].seedVars[j] == 1)
      {
        ii = data->simulationInfo->analyticJacobians[index].sparsePattern.leadindex[j];
        while(ii < data->simulationInfo->analyticJacobians[index].sparsePattern.leadindex[j+1])
        {
          l  = data->simulationInfo->analyticJacobians[index].sparsePattern.index[ii];
          k  = j*data->simulationInfo->analyticJacobians[index].sizeRows + l;
          jac[k] = data->simulationInfo->analyticJacobians[index].resultVars[l];
          ii++;
        };
      }
      /* de-activate seed variable for the corresponding color */
      if(data->simulationInfo->analyticJacobians[index].sparsePattern.colorCols[j]-1 == i)
        data->simulationInfo->analyticJacobians[index].seedVars[j] = 0;
    }

  }
#endif



/**
 * \brief Set up solver instance for one algebraic system.
 *
 * Allocate memory and set constant data like dimensions.
 *
 * \param [in]  alg_system  Algebraic system instance.
 * \return                  Returns `omsi_ok` on success, otherwise `omsi_error`.
 */
omsi_status omsi_set_up_solver (omsi_algebraic_system_t* alg_system) {

    /* Set callbacks */
    if (alg_system->isLinear) {
        solver_init_callbacks (global_callback->allocateMemory,
                               global_callback->freeMemory,
                               wrapper_lin_system_logger);
    }
    else {
        solver_init_callbacks (global_callback->allocateMemory,
                               global_callback->freeMemory,
                               wrapper_non_lin_system_logger);
    }

    /* Allocate memory */
    alg_system->solver_data = solver_allocate(solver_lapack, alg_system->n_iteration_vars);
    if (alg_system->solver_data == NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
            "fmi2Something: Can not allocate memory for solver instance.");
        return omsi_error;
    }

    /* Prepare specific solver data */
    if(prepare_specific_solver_data(alg_system->solver_data) != solver_ok) {
        solver_free(alg_system->solver_data);
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
            "fmi2Something: Could not prepare specific solver data for solver instance.");
        return omsi_error;
    }

    return omsi_ok;
}


/* ToDo: Add function to free solver instance */
void omsi_free_solver () {

}
