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

/** \file solver_kinsol.c
 *
 * Solver specific functions for kinsol solver.
 */

/** @addtogroup kinsol_SOLVER kinsol solver
 *  \ingroup NONLIN_SOLVER
  *  @{ */

#include <solver_kinsol.h>


/*
 * ============================================================================
 * Allocaten, Initialization and freeing of solver_specific_data
 * ============================================================================
 */


/**
 * Allocates memory for kinsol specific solver data and saves it in solver instance.
 *
 * \param [in,out]  general_solver_data     Solver instance.
 * \return          solver_status           solver_ok on success and
 *                                          solver_error on failure.
 */
solver_status solver_kinsol_allocate_data(solver_data* general_solver_data)
{
    /* Variables */
    solver_data_kinsol* kinsol_data;

    /* Check for correct solver */
    if (!solver_instance_correct(general_solver_data, solver_kinsol, "allocate_kinsol_data")) {
        return solver_error;
    }

    /* Check if general_solver_data has already specific data */
    if (general_solver_data->specific_data!=NULL) {
        solver_logger(log_solver_error, "In function allocate_kinsol_data: Pointer to solver specific data is not NULL.");
            general_solver_data->state = solver_error_state;
        return solver_error;
    }

    /* Allocate memory */
    kinsol_data = (solver_data_kinsol*) solver_allocateMemory(1, sizeof(solver_data_kinsol));
    if (!kinsol_data) {
        solver_logger(log_solver_error, "In function allocate_kinsol_data: Can't allocate memory for kinsol_data.");
        general_solver_data->specific_data = NULL;
        general_solver_data->state = solver_error_state;
        return solver_error;
    }

    /* Create Kinsol solver object */
    kinsol_data->kinsol_solver_object = KINCreate();
    if (kinsol_data->kinsol_solver_object == NULL) {
        solver_logger(log_solver_error, "In function allocate_kinsol_data: Could not create KINSOL solver object.");
        solver_freeMemory(kinsol_data);
        general_solver_data->specific_data = NULL;
        general_solver_data->state = solver_error_state;
        return solver_error;
    }

    kinsol_data->f_function_eval = NULL;
    kinsol_data->initial_guess = NULL;

    general_solver_data->specific_data = kinsol_data;
    general_solver_data->state = solver_instantiated;

    return solver_ok;
}


/**
 * \brief Set initial guess for vector `x`.
 *
 * \param [in,out]  general_solver_data     Solver instance.
 * \param [in]      initial_guess           Array with initial guess for vector `x` to start iteration.
 *                                          Has length general_solver_data->dim_n
 * \return          solver_status           solver_ok on success and
 *                                          solver_error on failure.
 */
solver_status solver_kinsol_set_start_vector (solver_data*  general_solver_data,
                                              solver_real*  initial_guess)
{
    /* Variables */
    solver_data_kinsol* kinsol_data;

    /* check for correct solver */
    if (!solver_instance_correct(general_solver_data, solver_kinsol, "solver_kinsol_free_data")) {
        return solver_error;
    }

    kinsol_data = general_solver_data->specific_data;

    /* Create initial_guess vector */
    kinsol_data->initial_guess = N_VMake_Serial(general_solver_data->dim_n, initial_guess);
    return solver_ok;
}


/**
 * Set dimension `dim_n` of function `f` in kinsol specific solver data.
 *
 * \param [in,out]  general_solver_data     Solver instance.
 * \param [in]      user_data               Pointer to user_data needed in user provided
 *                                          residual wrapper function. Can be `NULL`.
 * \return          solver_status           solver_ok on success and
 *                                          solver_error on failure.
 */
solver_status solver_kinsol_init_data(solver_data*              general_solver_data,
                                      residual_wrapper_func     user_wrapper_res_function,
                                      void*                     user_data)
{
    /* Variables */
    solver_data_kinsol* kinsol_data;
    solver_int flag;
    solver_unsigned_int i;
    solver_real* u_scale;
    solver_real* f_scale;

    /* check for correct solver */
    if (!solver_instance_correct(general_solver_data, solver_kinsol, "solver_kinsol_free_data")) {
        return solver_error;
    }

    /* Access data */
    kinsol_data = general_solver_data->specific_data;

    if (kinsol_data->initial_guess == NULL) {
        solver_logger(log_solver_error, "In function kinsol_init_data: Initial guess not set. "
                "Use API function solver_set_start_vector to set initial guess..");
        general_solver_data->state = solver_error_state;
        return solver_error;
    }

    /* Set KINSOL user data */
    kinsol_data->kin_user_data = (kinsol_user_data*) solver_allocateMemory(1, sizeof(kinsol_user_data));
    kinsol_data->kin_user_data->user_data = user_data;
    kinsol_data->kin_user_data->kinsol_data = kinsol_data;
    flag = KINSetUserData(kinsol_data->kinsol_solver_object, kinsol_data->kin_user_data);
    if (flag != KIN_SUCCESS) {
        solver_logger(log_solver_error, "In function kinsol_init_data: Could "
                "not set KINSOL user data.");
        general_solver_data->state = solver_error_state;
        return solver_error;
    }

    /* Set user supplied wrapper function */
    kinsol_data->f_function_eval = user_wrapper_res_function;

    /* Set problem-defining function and initialize KINSOL*/
    flag = KINInit(kinsol_data->kinsol_solver_object,
                   solver_kinsol_residual_wrapper,
                   kinsol_data->initial_guess);
    if (flag != KIN_SUCCESS) {
        solver_logger(log_solver_error, "In function kinsol_init_data: Could "
                "not initialize KINSOL solver object.");
        general_solver_data->state = solver_error_state;
        return solver_error;
    }

    /* Set KINSOL strategy */
    kinsol_data->strategy = KIN_FP;

    /* Create Jacobian matrix object */



    /* Create linear solver object */



    /* Attach linear solver module */


    /* Set scaling vectors */
    u_scale = (solver_real*) solver_allocateMemory(general_solver_data->dim_n, sizeof(solver_real));
    f_scale = (solver_real*) solver_allocateMemory(general_solver_data->dim_n, sizeof(solver_real));
    for (i=0; i<general_solver_data->dim_n; i++) {
        u_scale[i] = 1;                 /* ToDo: Do smarter stuff here */
        f_scale[i] = 1;
    }
    kinsol_data->u_scale = N_VMake_Serial(general_solver_data->dim_n, u_scale);
    kinsol_data->f_scale = N_VMake_Serial(general_solver_data->dim_n, f_scale);


    /* Set state and exit*/
    general_solver_data->state = solver_initializated;
    return solver_ok;
}


/**
 *  \brief Frees kinsol specific solver data.
 *
 * \param [in,out]  general_solver_data     Solver instance.
 * \return          solver_status           solver_ok on success and
 *                                          solver_error on failure.
 */
solver_status solver_kinsol_free_data(solver_data* general_solver_data)
{
    /* Variables */
    solver_data_kinsol* kinsol_data;

    /* check for correct solver */
    if (!solver_instance_correct(general_solver_data, solver_kinsol, "kinsol_free_data")) {
        return solver_error;
    }

    kinsol_data = general_solver_data->specific_data;

    /* Free data */
    KINFree((void*)kinsol_data);
    solver_freeMemory(kinsol_data->kin_user_data);

    solver_freeMemory(NV_DATA_S(kinsol_data->initial_guess));       /* ToDo: Is it smart to free a user supplied aray???
                                                                       Well the free Function is also provided by user, so it should work any way...
                                                                       Maybe... */
    N_VDestroy_Serial(kinsol_data->initial_guess);

    solver_freeMemory(NV_DATA_S(kinsol_data->u_scale));
    N_VDestroy_Serial(kinsol_data->u_scale);

    solver_freeMemory(NV_DATA_S(kinsol_data->f_scale));
    N_VDestroy_Serial(kinsol_data->f_scale);

    solver_freeMemory(kinsol_data);

    /* Set solver state */
    general_solver_data->state = solver_uninitialized;
    return solver_ok;
}


/*
 * ============================================================================
 * Kinsol wrapper functions
 * ============================================================================
 */

/**
 * \brief Computes system function `f` of non-linear problem.
 *
 * This function is of type `KINSysFn` and will be used by Kinsol solver
 * to evaluate `f(x)`
 *
 * \param [in]      x           Dependent variable vector `x`
 * \param [out]     fval        Set fval to `f(x)`.
 * \param [in,out]  userData
 * \return          solver_int  Return value is ignored from Kinsol.
 */
solver_int solver_kinsol_residual_wrapper(N_Vector  x,
                                          N_Vector  fval,
                                          void*     user_data_in) {

    /* Variables */
    solver_data_kinsol* kinsol_data;
    solver_real* x_data;
    solver_real* fval_data;
    kinsol_user_data* user_data;

    /* Access input data */
    user_data = (kinsol_user_data*) user_data_in;
    kinsol_data = user_data->kinsol_data;
    x_data = NV_DATA_S(x);
    fval_data = NV_DATA_S(fval);


    /* Call residual function */
    kinsol_data->f_function_eval(x_data, fval_data, user_data->user_data);

    /* Log function call */


    return 0;
}


/*
 * ============================================================================
 * Solve call
 * ============================================================================
 */

solver_state solver_kinsol_solve(void* specific_data)
{
    /* Variables */
    solver_data_kinsol* kinsol_data;
    solver_int flag;

    kinsol_data = specific_data;

    /* Solver call */
    flag = KINSol(kinsol_data->kinsol_solver_object,
                  kinsol_data->initial_guess,
                  kinsol_data->strategy,
                  kinsol_data->u_scale,
                  kinsol_data->f_scale);

    switch (flag) {
        case KIN_SUCCESS:
            return solver_ok;
        case KIN_INITIAL_GUESS_OK:
            return solver_ok;
        case KIN_STEP_LT_STPTOL:
            solver_logger(log_solver_error,
                    "In function solver_kinsol_solve: Kinsol stopped based on "
                    "scaled step length. It is possible that the solution is "
                    "within tolerances specified or the algorithm is stalled "
                    "near an invalid solution or that scalar scsteptol is too "
                    "large.");
            return solver_warning;
        case KIN_MEM_NULL:
            solver_logger(log_solver_error,
                    "In function solver_kinsol_solve: The Kinsol memory block "
                    "pointer was NULL.");
            return solver_error;
        case KIN_ILL_INPUT:
            solver_logger(log_solver_error,
                    "In function solver_kinsol_solve: An input parameter was "
                    "invalid.");
            return solver_error;
        case KIN_NO_MALLOC:
            solver_logger(log_solver_error,
                    "In function solver_kinsol_solve: The KINSOL memory was not "
                    "allocated by a call to KINCreate.");
            return solver_error;
        case KIN_MEM_FAIL:
            solver_logger(log_solver_error,
                    "In function solver_kinsol_solve: A memory allocation failed.");
            return solver_error;
        case KIN_LINESEARCH_NONCONV:
            solver_logger(log_solver_error,
                    "In function solver_kinsol_solve: The line search algorithm "
                    "was unable to find an iterate sufficiently distinct from "
                    "current iterate, or could not find an iterate satisfying "
                    "the sufficient decrease condition.");
            return solver_error;
        case KIN_MAXITER_REACHED:
            solver_logger(log_solver_error,
                    "In function solver_kinsol_solve: The maximum number of "
                    "nonlinear iterations has been reached.");
            return solver_error;
        case KIN_MXNEWT_5X_EXCEEDED:
            solver_logger(log_solver_error,
                "In function solver_kinsol_solve: Five consecutive steps have "
                "been taken with L2_norm(D_u*p)>0.99*mxnewstep");
            return solver_error;
        case KIN_LINESEARCH_BCFAIL:
            solver_logger(log_solver_error,
                "In function solver_kinsol_solve: Linear search was unable to "
                "satisfy beta-condition. Algorithm may is making poor progress.");
            return solver_warning;
        case KIN_LINSOLV_NO_RECOVERY:
            return solver_error;
        case KIN_LINIT_FAIL:
            return solver_error;
        case KIN_LSETUP_FAIL:
            return solver_error;
        case KIN_LSOLVE_FAIL:
            return solver_error;
        case KIN_SYSFUNC_FAIL:
            return solver_error;
        case KIN_FIRST_SYSFUNC_ERR:
            return solver_warning;
        case KIN_REPTD_SYSFUNC_ERR:
            return solver_error;
        default:
            solver_logger(log_solver_error,
                    "In function solver_kinsol_solve: Something went wrong...");
            return solver_error;
        /* ToDo: Add more error cases */
    }
}



/*
 * ============================================================================
 * Getters and setters
 * ============================================================================
 */

void solver_kinsol_get_x_element(void*                  specific_data,
                                 solver_unsigned_int    index,
                                 solver_real*           value)
{
    /* Variables */
    solver_data_kinsol* kinsol_data;
    solver_real* x_vector;

    kinsol_data = specific_data;

    /* Access initial_guess(index) */
    x_vector = N_VGetArrayPointer(kinsol_data->initial_guess);
    value[0]=x_vector[index];
}









/** @} */
