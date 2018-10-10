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

#ifndef OMSI_CALLBACKS_H
#define OMSI_CALLBACKS_H

#include <omsi.h>

#ifdef __cplusplus
extern "C" {
#endif

int evaluate_ode(omsi_t);
int evaluate_outputs(omsi_t);
int evaluate_zerocrossings(omsi_t);
int evaluate_discrete_system(omsi_t);
int evaluate_bound_parameter(omsi_t);
int evaluate_intial_system(omsi_t);


/*
 * OMSI callback functions
 */
typedef void      (*omsi_callback_logger)           (const void*,       /* component environment */
                                                     omsi_string,       /* instance name */
                                                     omsi_status,       /* status */
                                                     omsi_string,       /* category */
                                                     omsi_string, ...); /* message, ... */
typedef void*     (*omsi_callback_allocate_memory)  (omsi_unsigned_int, omsi_unsigned_int);
typedef void      (*omsi_callback_free_memory)      (void*);
typedef void      (*omsi_step_finished)             (void*, omsi_status);

typedef struct omsi_callback_functions{
    const omsi_callback_logger          logger;
    const omsi_callback_allocate_memory allocateMemory;
    const omsi_callback_free_memory     freeMemory;
    const omsi_step_finished            stepFinished;
    void*                               componentEnvironment;
}omsi_callback_functions;


/*
 * Callbacks for template functions
 */
typedef omsi_status (*omsu_initialize_omsi_function) (omsi_function_t* omsi_function);
typedef omsi_status (*evaluate_function) (omsi_function_t* function, omsi_values* variables, void* data);


typedef struct omsi_template_callback_functions_t {
    omsi_bool isSet;                                                        /* Boolean if template functions are set */
    omsu_initialize_omsi_function initialize_initialization_problem;  /* function pointer to initialize the initialization problem */
    omsu_initialize_omsi_function initialize_simulation_problem;      /* function pointer to initialize the simulation problem */

}omsi_template_callback_functions_t;


#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
