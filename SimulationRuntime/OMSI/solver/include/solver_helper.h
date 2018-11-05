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

#ifndef _SOLVERHELPER_H_
#define _SOLVERHELPER_H_

#include <omsi.h>
#include <omsi_callbacks.h>
#include <omsi_global.h>
#include <omsi_utils.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


/* MACROS */
#define CHECK_MEMORY_ERROR(pointer)                                           \
if (!pointer) {                                                               \
    /* ToDo: Log error */                                                     \
    return omsi_error;                                                        \
}                                                                             \


/* Function prototypes */
omsi_status free_omsi_function (omsi_function_t*    omsi_function,
                                omsi_bool           shared_function_vars);

omsi_status free_alg_system (omsi_algebraic_system_t* algebraic_system);

omsi_status free_omsi_values (omsi_values* values);

omsi_status omsu_set_omsi_value (omsi_values*       vars,
                                 omsi_index_type**  targetPlaces,
                                 omsi_unsigned_int  numTargetPlaces,
                                 omsi_real          targetValue);

omsi_values* save_omsi_values (const omsi_values*   vars,
                               omsi_index_type**    saveTarget,
                               omsi_unsigned_int    n_reals,
                               omsi_unsigned_int    n_ints,
                               omsi_unsigned_int    n_bools);

omsi_status omsi_get_derivative_matrix (omsi_algebraic_system_t*    algebraicSystem,
                                        omsi_bool                   rowMajorOrder,
                                        omsi_real*                  matrix);
#ifdef __cplusplus
}   /* end of extern "C" { */
#endif
#endif

