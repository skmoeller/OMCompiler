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
 * Interface (OMSI). These functions are used for instantiation and initialization
 * of the FMU.
 */

#ifndef OMSU_INITIALIZATION__H_
#define OMSU_INITIALIZATION__H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "omsu_helper.h"
#include "omsu_common.h"
#include "omsu_utils.h"
#include "omsu_me.h"
#include "fmi2/fmi2Functions.h"

/* forward struct */
typedef struct osu_t osu_t;
typedef struct omsi_t omsi_t;

/* extern functions */
extern void omsic_model_setup_data(osu_t* OSU);


/*! \fn omsi_me_instantiate
 *
 *  This function instantiates the osu instance.
 *
 *  \param [ref] [data]
 */
fmi2Component omsi_instantiate(fmi2String    instanceName,
                               fmi2Type      fmuType,
                               fmi2String    fmuGUID,
                               fmi2String    fmuResourceLocation,
                               const fmi2CallbackFunctions* functions,
                               fmi2Boolean                  visible,
                               fmi2Boolean                  loggingOn);


/*! \fn omsi_enter_initialization_mode
 *
 *  This function enters initialization mode.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_enter_initialization_mode(fmi2Component c);

/*! \fn omsi_exit_initialization_mode
 *
 *  This function exits initialization mode.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_exit_initialization_mode(fmi2Component c);

/*! \fn omsi_setup_experiment
 *
 *  This function sets up an experiment.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_setup_experiment(fmi2Component c,
                                 fmi2Boolean   toleranceDefined,
                                 fmi2Real      tolerance,
                                 fmi2Real      startTime,
                                 fmi2Boolean   stopTimeDefined,
                                 fmi2Real      stopTime);

/*! \fn omsi_free_instance
 *
 *  This function frees the osu instance.
 *
 *  \param [ref] [data]
 */
void omsi_free_instance(fmi2Component c);

/*! \fn omsi_reset
 *
 *  This function resets the instance.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_reset(fmi2Component c);

/*! \fn omsi_terminate
 *
 *  This function deinitializes the instance.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_terminate(fmi2Component c);

#ifdef __cplusplus
}
#endif
#endif
