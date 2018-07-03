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

#ifndef _OMSI_EQNS_SYSTEM_H
#define _OMSI_EQNS_SYSTEM_H

#include "omsi.h"
/* #include "omsi_jacobian.h" */


#ifdef __cplusplus
extern "C" {
#endif


/* forward some types from omsi.h */
typedef struct sim_data_t sim_data_t;
typedef struct equation_info_t equation_info_t;
typedef struct omsi_function_t omsi_function_t;
typedef struct omsi_index_type omsi_index_type;

/* forward some types */
typedef struct omsi_linear_system_t omsi_linear_system_t;
typedef struct omsi_sparsity_pattern omsi_sparsity_pattern;
typedef struct omsi_analytical_jacobian omsi_analytical_jacobian;

/* function prototypes for omsi_linear_system_t functions */
/* typedef omsi_int (*omsi_linear_system_t_get_x)(sim_data_t* data, omsi_vector_t* vector); */


/**
 *
 */
typedef struct omsi_analytical_jacobian{
    omsi_unsigned_int n_columns;
    omsi_unsigned_int n_rows;

    omsi_function_t* functions;
    /* sparsity_pattern_t* sparsity_pattern; */
}omsi_analytical_jacobian;

/**
 * general algebraic system
 */
typedef struct {
    omsi_unsigned_int n_iteration_vars;
    omsi_index_type* iteration_vars_indices;

    omsi_unsigned_int n_conditions;
    omsi_int* zerocrossing_indices;

    omsi_bool isLinear;         /* linear system=true and non-linear system=false */
    void* loop;                 /* points on array of omsi_linear_loop or omsi_non-linear_loop */

    omsi_function_t* functions;

}omsi_algebraic_system_t;


/**
 * linear system
 */
typedef struct {
    /*model_variables_info_t *info;*/
    void* solverData;
    omsi_analytical_jacobian *jacobian;

}omsi_linear_loop;

/**
 * non-linear system
 */
typedef struct {
    /* ToDo: complete */
}omsi_nonlinear_loop;


#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
