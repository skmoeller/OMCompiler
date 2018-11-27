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
solver_status kinsol_allocate_data(solver_data* general_solver_data) {

    /* Variables */
    solver_data_kinsol* kinsol_data;
    solver_int flag;
    KINSysFn func;
    N_Vector tmpl;

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
                    general_solver_data->state = solver_error_state;
        general_solver_data->state = solver_error_state;
        return solver_error;
    }

    /* Set problem-defining function and initialize KINSOL*/
    flag = KINInit(kinsol_data->kinsol_solver_object, func, tmpl);
    if (flag != KIN_SUCCESS) {
        solver_logger(log_solver_error, "In function allocate_kinsol_data: Could not initialize KINSOL solver object.");
                    general_solver_data->state = solver_error_state;
        general_solver_data->state = solver_error_state;
        return solver_error;
    }

    general_solver_data->specific_data = kinsol_data;

    return solver_ok;
}


/**
 *  Frees kinsol specific solver data.
 *
 * \param [in,out]  general_solver_data     Solver instance.
 * \return          solver_status           solver_ok on success and
 *                                          solver_error on failure.
 */
solver_status kinsol_free_data(solver_data* general_solver_data) {

    /* Variables */
    solver_data_kinsol* kinsol_data;

    /* check for correct solver */
    if (!solver_instance_correct(general_solver_data, solver_kinsol, "kinsol_free_data")) {
        return solver_error;
    }

    kinsol_data = general_solver_data->specific_data;

    /* Free KINSOL solver object */
    KINFree((void*)kinsol_data);

    return solver_ok;
}


/**
 * Set dimension `dim_n` of function `f` in kinsol specific solver data.
 *
 * \param [in,out]  general_solver_data     Solver instance.
 * \return          solver_status           solver_ok on success and
 *                                          solver_error on failure.
 */
solver_status kinsol_set_dim_data(solver_data* general_solver_data) {


    return solver_ok;
}


/** @} */
