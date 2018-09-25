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


#define UNUSED(x) (void)(x)     /* ToDo: delete later */



omsi_status omsi_allocate_model_variables(omsi_t*                           omsu,
                                          const omsi_callback_functions*    functions) {

    /* Variables */
    omsi_unsigned_int n_bools, n_ints, n_reals;


    /* set global function pointer */
    global_callback = (omsi_callback_functions*) functions;

    LOG_FILTER(global_callback->componentEnvironment, LOG_ALL,
        global_callback->logger(global_callback->componentEnvironment, global_instance_name, omsi_ok,
        logCategoriesNames[LOG_ALL], "fmi2Instantiate: Allocate memory for model_variables.."))

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

omsi_status omsi_initialize_model_variables(omsi_t* omsu, const omsi_callback_functions*  functions, omsi_string instanceName) {

    if (!omsu->model_data) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
            "no model data available");
        return omsi_error;
    }
    if (!omsu->model_data->model_vars_info) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
            "no model vars info available");
        return omsi_error;
    }
    if (!omsu->sim_data->model_vars_and_params) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
            "no model vars and parameter structure is not yet allocated");
        return omsi_error;
    }
    if (!omsu->sim_data->model_vars_and_params->reals) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
            "real variables are not yet allocated");
        return omsi_error;
    }
    if (!omsu->sim_data->model_vars_and_params->ints) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
            "int variables are not yet allocated");
        return omsi_error;
    }
    if (!omsu->sim_data->model_vars_and_params->bools) {
        functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
            "bool variables are not yet allocated");
        return omsi_error;
    }
    /*Initialize state variables from init xml file values*/
    int state;
    int n = omsu->model_data->n_states;
    for (state = 0; state <n; ++state) {
        real_var_attribute_t* attr = (real_var_attribute_t*)(omsu->model_data->model_vars_info[state].modelica_attributes);
        if (!attr) {
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
                "could not read start value attribute");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->reals[state] = attr->start;
    }
    /*Initialize derivatives variables from init xml file values*/
    int derstate;
    n = n + omsu->model_data->n_derivatives;
    for (derstate = state; derstate <n; ++derstate) {
        real_var_attribute_t* attr = (real_var_attribute_t*)(omsu->model_data->model_vars_info[derstate].modelica_attributes);
        if (!attr) {
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
                "could not read start value attribute");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->reals[derstate] = attr->start;
    }
    /*Initialize real algebraic variables from init xml file values*/
    int real_algebraic;
    n = n + omsu->model_data->n_real_vars;
    for (real_algebraic = derstate; real_algebraic < n; ++real_algebraic) {
        real_var_attribute_t* attr = (real_var_attribute_t*)(omsu->model_data->model_vars_info[real_algebraic].modelica_attributes);
        if (!attr) {
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
                "could not read start value attribute");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->reals[real_algebraic] = attr->start;
    }

    /*Initialize real parameter variables from init xml file values*/
    int real_parameter;
    n = n + omsu->model_data->n_real_parameters;
    for (real_parameter = real_algebraic; real_parameter < n; ++real_parameter) {
        real_var_attribute_t* attr = (real_var_attribute_t*)(omsu->model_data->model_vars_info[real_parameter].modelica_attributes);
        if (!attr) {
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
                "could not read start value attribute");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->reals[real_parameter] = attr->start;
    }
    /*real alias variables are not extra included in real vars memory,therefore they are skipped*/
    int real_alias = omsu->model_data->n_real_aliases;
    n = n + real_alias;
    /*Initialize int algebraic variables from init xml file values*/
    int int_algebraic;
    int int_algebraic_2;
    n = n + omsu->model_data->n_int_vars;
    for (int_algebraic = real_parameter+ real_alias, int_algebraic_2=0; int_algebraic < n; ++int_algebraic, ++int_algebraic_2) {
        int_var_attribute_t* attr = (int_var_attribute_t*)(omsu->model_data->model_vars_info[int_algebraic].modelica_attributes);
        if (!attr) {
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
                "could not read start value attribute");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->ints[int_algebraic_2] = attr->start;
    }
    /*Initialize int parameter from init xml file values*/
    int int_parameter;
    int int_parameter_2;
    n = n + omsu->model_data->n_int_parameters;
    for (int_parameter = int_algebraic, int_parameter_2 = int_algebraic_2; int_parameter < n; ++int_parameter, ++int_parameter_2) {
        int_var_attribute_t* attr = (int_var_attribute_t*)(omsu->model_data->model_vars_info[int_parameter].modelica_attributes);
        if (!attr) {
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
                "could not read start value attribute");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->ints[int_parameter_2] = attr->start;
    }
    /*int alias variables are not extra included in int vars memory,therefore they are skipped*/
    int int_alias = omsu->model_data->n_int_aliases;
    n = n + int_alias;
    /*Initialize bool algebraic variables from init xml file values*/
    int bool_algebraic;
    int bool_algebraic_2;
    n = n + omsu->model_data->n_bool_vars;
    for (bool_algebraic = int_parameter+ int_alias, bool_algebraic_2 = 0; bool_algebraic < n; ++bool_algebraic, ++bool_algebraic_2) {
        bool_var_attribute_t* attr = (bool_var_attribute_t*)(omsu->model_data->model_vars_info[bool_algebraic].modelica_attributes);
        if (!attr) {
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
                "could not read start value attribute");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->bools[bool_algebraic_2] = attr->start;
    }
    /*Initialize bool parameter  from init xml file values*/
    int bool_parameter;
    int bool_parameter_2;
    n = n + omsu->model_data->n_bool_parameters;
    for (bool_parameter = bool_algebraic, bool_parameter_2 = bool_algebraic_2; bool_parameter < n; ++bool_parameter, ++bool_parameter_2) {
        bool_var_attribute_t* attr = (bool_var_attribute_t*)(omsu->model_data->model_vars_info[bool_parameter].modelica_attributes);
        if (!attr) {
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error",
                "could not read start value attribute");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->bools[bool_parameter_2] = attr->start;
    }
    /*Todo: Initialize string algebraic variables  from init xml file values*/

    return omsi_ok;
}

omsi_status omsi_free_model_variables(omsi_t* omsu) {

    alignedFree(omsu->sim_data->model_vars_and_params->bools);
    alignedFree(omsu->sim_data->pre_vars->bools);
    alignedFree(omsu->sim_data->model_vars_and_params->ints);
    alignedFree(omsu->sim_data->pre_vars->ints);
    alignedFree(omsu->sim_data->model_vars_and_params->reals);
    alignedFree(omsu->sim_data->pre_vars->reals);

    return omsi_ok;
}


void* alignedMalloc(size_t required_bytes, size_t alignment)        /* ToDo: change size_t to some omsi type */
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
