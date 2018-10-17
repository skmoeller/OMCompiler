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

#ifndef OMSU_HELPER_H
#define OMSU_HELPER_H

#include <stdio.h>
#include <string.h>
#include <stddef.h>


#include <omsic.h>

#include <solver_lapack.h>

#ifdef __cplusplus
extern "C" {
#endif



/* extern function prototypes */
extern void printLapackData(DATA_LAPACK*    lapack_data,
                            omsi_string     indent);


/* function prototypes */
omsi_string stateToString(osu_t* OSU);

omsi_bool invalidState(osu_t*       OSU,
                       omsi_string  function_name,
                       omsi_int     meStates,
                       omsi_int     csStates);

omsi_bool nullPointer(osu_t*        OSU,
                      omsi_string   function_name,
                      omsi_string   arg,
                      const void *  pointer);

omsi_bool vrOutOfRange(osu_t*               OSU,
                       omsi_string          function_name,
                       omsi_unsigned_int    vr,
                       omsi_int             end);

omsi_status unsupportedFunction(osu_t*      OSU,
                                omsi_string function_name,
                                omsi_int    statesExpected);

omsi_bool invalidNumber(osu_t*          OSU,
                        omsi_string     function_name,
                        omsi_string     arg,
                        omsi_int        n,
                        omsi_int        nExpected);

omsi_status omsi_set_debug_logging(osu_t*               OSU,
                                   omsi_bool            loggingOn,
                                   omsi_unsigned_int    nCategories,
                                   const omsi_string    categories[]);

omsi_bool omsu_discrete_changes(osu_t*  OSU,
                                void*   threadData);

void omsu_storePreValues(omsi_t* omsi_data);

omsi_bool omsu_values_equal(omsi_values*    vars_1,
                            omsi_values*    vars_2);

omsi_status omsu_copy_values(omsi_values*   target_vars,
                             omsi_values*   source_vars);

void omsu_print_osu (osu_t* OSU);

void omsu_print_omsi_t (omsi_t*     omsi,
                        omsi_string indent);

void omsu_print_model_data(model_data_t*    model_data,
                           omsi_string      indent);

omsi_status omsu_print_model_variable_info(model_data_t*  model_data,
                                           omsi_string    indent);

omsi_status omsu_print_modelica_attributes (void*               modelica_attribute,
                                            omsi_index_type*    type_index,
                                            omsi_string         indent);

void omsu_print_real_var_attribute (real_var_attribute_t*   real_var_attribute,
                                    omsi_string             indent);

void omsu_printf_int_var_attribute(int_var_attribute_t* int_var_attribute,
                                   omsi_string          indent);

omsi_status omsu_print_equation_info(model_data_t*  model_data,
                                     omsi_string    indent);

void omsu_print_experiment (omsi_experiment_t*  experiment,
                            omsi_string         indent);

omsi_status omsu_print_sim_data (sim_data_t* sim_data,
                                 omsi_string indent);

omsi_status omsu_print_omsi_function_rec (omsi_function_t* omsi_function,
                                          omsi_string      omsi_function_name,
                                          omsi_string      indent);

omsi_status omsu_print_this_omsi_function (omsi_function_t* omsi_function,
                                           omsi_string      omsi_function_name,
                                           omsi_string      indent);

omsi_status omsu_print_omsi_values (omsi_values*        omsi_values,
                                    omsi_string         omsi_values_name,
                                    omsi_string         indent);

omsi_status omsu_print_algebraic_system(omsi_algebraic_system_t*    algebraic_system_t,
                                        omsi_string                 indent);

omsi_status omsu_print_index_type (omsi_index_type*     vars_indices,
                                   omsi_unsigned_int    size,
                                   omsi_string          indent);

omsi_status omsu_print_externs(void*                externs,
                               omsi_unsigned_int    n_externs);

omsi_status omsu_print_solver_data(omsi_string  solver_name,
                                   void*        solver_data,
                                   omsi_string  indent);
#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
