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




omsi_status omsi_solve_algebraic_system (omsi_algebraic_system_t*   alg_system,
                                         const omsi_values*         read_only_model_vars_and_params) {

    /* Check if solver is ready */
    if (alg_system->solver_data == NULL ) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_fatal,
            "fmi2Evaluate: Solver for %s system %u not set.",
            alg_system->isLinear ? "linear":"nonlinear",
            alg_system->id);
        return omsi_fatal;
    }

    /* Log solver call */
    filtered_base_logger(global_logCategories, log_all, omsi_ok,
        "fmi2Evaluate: Solve %s system %u of size %u with solver %s.",
        alg_system->isLinear ? "linear":"nonlinear",
        alg_system->id,
        alg_system->n_iteration_vars,
        solver_get_name(alg_system->solver_data));


    /* Check if it is possible to reuse matrix A or vector b */


    /* Update solver specific data */
    omsi_get_analytical_jacobian(alg_system, read_only_model_vars_and_params);
    omsi_get_right_hand_side(alg_system, read_only_model_vars_and_params);

    alg_system->solver_data->state = solver_ready;

    /* Call solver */
    print_solver_data(alg_system->solver_data, "fmi2Evaluate: Debug print");          /* ToDo: delete */

    solver_linear_solve(alg_system->solver_data);

    print_solver_data(alg_system->solver_data, "fmi2Evaluate: Debug print");     /* ToDo: delete */


    /* Save results */


    return omsi_ok;
}


omsi_status omsi_get_analytical_jacobian (omsi_algebraic_system_t*  alg_system,
                                          const omsi_values*        read_only_model_vars_and_params) {

    /* Variables */
    omsi_unsigned_int i;
    omsi_unsigned_int seed_index;

    /* Set seed vars */
    for (i=0; i<alg_system->jacobian->n_input_vars; i++) {
        seed_index = alg_system->jacobian->input_vars_indices[i].index;
        alg_system->jacobian->local_vars->reals[seed_index] = 0;
    }



    /* Build jacobian row wise with directional derivatives */  /* ToDo: check if realy row wise and not column wise... */
    for (i=0; i<alg_system->jacobian->n_output_vars; i++) {    /* ToDo: Add coloring here */
        /* Activate seed for current row*/
        seed_index = alg_system->jacobian->input_vars_indices[i].index;
        alg_system->jacobian->local_vars->reals[seed_index] = 1;

        /* Evaluate directional derivative */
        alg_system->jacobian->evaluate(alg_system->jacobian, read_only_model_vars_and_params, NULL);

        /* Set i-th row of matrix A */
        set_matrix_A(alg_system->solver_data,
                     NULL, alg_system->jacobian->n_output_vars,
                     &i, 1,
                     &alg_system->jacobian->local_vars->reals[alg_system->jacobian->output_vars_indices[0].index]);

        /* Reset seed vector */
        alg_system->jacobian->local_vars->reals[seed_index] = 0;
    }

    return omsi_ok;
}


omsi_status omsi_get_right_hand_side (omsi_algebraic_system_t*  alg_system,
                                      const omsi_values*        read_only_model_vars_and_params) {

    /* Variables */
    omsi_real* residual;
    omsi_unsigned_int i, n_loop_iteration_vars;
    omsi_unsigned_int loop_iter_var_index;

    n_loop_iteration_vars = alg_system->jacobian->n_output_vars;

    /* Allocate memory */
    residual = global_callback->allocateMemory(alg_system->jacobian->n_input_vars, sizeof(omsi_real));

    /* Set loop iteration variables zero. */
    for (i=0; i<n_loop_iteration_vars; i++) {
        loop_iter_var_index = alg_system->functions->output_vars_indices[i].index;
        alg_system->functions->function_vars->reals[loop_iter_var_index] = 0;
    }

    /* Evaluate residuum function */
    alg_system->functions->evaluate(alg_system->functions, read_only_model_vars_and_params, residual);

    set_vector_b(alg_system->solver_data, NULL, alg_system->jacobian->n_input_vars, residual);


    /* Free memory */
    global_callback->freeMemory(residual);

    return omsi_ok;
}



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
