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

#include <omsi_input_model_variables.h>
#include <omsi_global.h>


#define UNUSED(x) (void)(x)     /* ToDo: delete later */



omsi_int omsi_allocate_model_variables(omsi_t*                           omsu,
                                       const omsi_callback_functions*    functions) {

    /* Variables */
    omsi_unsigned_int n_bools, n_ints, n_reals;



    /* set global function pointer */
    global_callback = functions;

    /*Todo: Allocate memory for all string model variables*/

    /*Allocate memory for all boolean model variables*/
    n_bools = omsu->model_data->n_bool_vars+ omsu->model_data->n_bool_parameters;
    if (n_bools > 0) {
        omsu->sim_data->model_vars_and_params->bools = (omsi_bool*)alignedMalloc(sizeof(omsi_bool) * n_bools, 64);
        omsu->sim_data->pre_vars->bools = (omsi_bool*)alignedMalloc(sizeof(omsi_bool) * n_bools, 64);
        omsu->sim_data->model_vars_and_params->n_bools = n_bools;
        omsu->sim_data->pre_vars->n_bools = n_bools;
    }
    else {
        omsu->sim_data->model_vars_and_params->bools = NULL;
        omsu->sim_data->pre_vars->bools = NULL;
        omsu->sim_data->model_vars_and_params->n_bools = 0;
        omsu->sim_data->pre_vars->n_bools = 0;
    }

    /*Allocate memory for all integer model variables*/
    n_ints = omsu->model_data->n_int_vars+ omsu->model_data->n_int_parameters;
    if (n_ints > 0) {
        omsu->sim_data->model_vars_and_params->ints = (omsi_int*)alignedMalloc(sizeof(omsi_int) * n_ints, 64);
        omsu->sim_data->pre_vars->ints =  (omsi_int*)alignedMalloc(sizeof(omsi_int) * n_ints, 64);
        omsu->sim_data->model_vars_and_params->n_ints = n_ints;
        omsu->sim_data->pre_vars->n_ints = n_ints;
    }
    else {
        omsu->sim_data->model_vars_and_params->ints = NULL;
        omsu->sim_data->pre_vars->ints = NULL;
        omsu->sim_data->model_vars_and_params->n_ints = 0;
        omsu->sim_data->pre_vars->n_ints = 0;
    }

    /*Allocate memory for all real model variables*/
    n_reals =  omsu->model_data->n_states + omsu->model_data->n_derivatives + omsu->model_data->n_real_vars + omsu->model_data->n_real_parameters;
    if (n_reals > 0) {
        omsu->sim_data->model_vars_and_params->reals = (omsi_real*)alignedMalloc(sizeof(omsi_real) * n_reals, 64);
        omsu->sim_data->pre_vars->reals = (omsi_real*)alignedMalloc(sizeof(omsi_real) * n_reals, 64);
        omsu->sim_data->model_vars_and_params->n_reals = n_reals;
        omsu->sim_data->pre_vars->n_reals = n_reals;
    }
    else {
        omsu->sim_data->model_vars_and_params->reals = NULL;
        omsu->sim_data->model_vars_and_params->n_reals = 0;
        omsu->sim_data->pre_vars->reals = NULL;
        omsu->sim_data->pre_vars->n_reals = 0;
    }

    return omsi_ok;
}


omsi_int omsi_free_model_variables(omsi_t*                           omsu,
									const omsi_callback_functions*    functions) {



    alignedFree(omsu->sim_data->model_vars_and_params->bools);
    alignedFree(omsu->sim_data->pre_vars->bools);
    alignedFree(omsu->sim_data->model_vars_and_params->ints);
    alignedFree(omsu->sim_data->pre_vars->ints);
    alignedFree(omsu->sim_data->model_vars_and_params->reals);
    alignedFree(omsu->sim_data->pre_vars->reals);

    return omsi_ok;
}


void* alignedMalloc(size_t required_bytes, size_t alignment)		/* ToDo: change size_t to some omsi type */
{
    void *p1;
    void **p2;

    int offset = alignment - 1 + sizeof(void*);
    p1 = global_callback->allocateMemory(1,required_bytes + offset);
    p2 = (void**)(((size_t)(p1)+offset)&~(alignment - 1));
    p2[-1] = p1;
    return p2;
}

void alignedFree(void* p)
{
    void* p1 = ((void**)p)[-1];         /* get the pointer to the buffer we allocated */
    global_callback->freeMemory(p1);
}
