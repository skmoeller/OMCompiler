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

/*
 * This file defines functions for the FMI used via the OpenModelica Simulation
 * Interface (OMSI). These are helper functions used for the other omsu functions.
 */

#ifndef OMSU_UTILS__H_
#define OMSU_UTILS__H_


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <omsi.h>
#include <omsi_callbacks.h>
#include <omsi_global.h>


#ifdef __cplusplus
extern "C" {
#endif

#if _MSC_VER
#define __attribute__(x)
#endif


/* function prototypes */
void filtered_base_logger(omsi_bool*            logCategories,      /* Array of categories, that should be logged, can be NULL */
                          log_categories        category,           /* Category of this log call */
                          omsi_status           status,             /* Status for logger */
                          omsi_string           message,            /* Message for logger */
                          ...);                                      /* Optional arguments in message */

omsi_bool isCategoryLogged(omsi_bool*       logCategories,
                           log_categories   categoryIndex);

void omsu_free_osu_data(omsi_t* omsi_data);

void omsu_free_model_data (model_data_t* model_data);

void omsu_free_sim_data (sim_data_t* sim_data);

void omsu_free_omsi_function(omsi_function_t* omsi_function);

void omsu_free_omsi_values(omsi_values* values);

#ifdef __cplusplus
}
#endif
#endif
