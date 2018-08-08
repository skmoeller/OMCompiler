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

#include "omsi.h"
#include "omsu_common.h"

#include <stdio.h>
#include <string.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


/* function prototypes */
void omsu_free_osu_data(omsi_t*                         omsi_data,
                        const omsi_callback_free_memory freeMemory);

omsi_status omsu_allocate_sim_data(omsi_t*                              omsi_data,
                                   const omsi_callback_allocate_memory  allocateMemory);

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

void omsu_print_sim_data (sim_data_t* sim_data,
                          omsi_string indent);


#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
