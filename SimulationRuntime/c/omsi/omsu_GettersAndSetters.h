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
 * Interface (OMSI). These are the common functions for getting and setting
 * variables and FMI informations.
 */

#ifndef OMSU_GETTERSANDSETTERS__H_
#define OMSU_GETTERSANDSETTERS__H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "omsu_utils.h"
#include "fmi2/fmi2Functions.h"

#ifdef __cplusplus
extern "C" {
#endif

//getter and setter functions for the omsu struct

/*! \fn omsi_get_boolean
 *
 *  This function obtains booleans of the model.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_get_boolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[]);

/*! \fn omsi_get_integer
 *
 *  This function obtains integers of the model.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_get_integer(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[]);

/*! \fn omsi_get_real
 *
 *  This function obtains reals of the model.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_get_real(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[]);

/*! \fn omsi_get_string
 *
 *  This function obtains strings of the model.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_get_string(fmi2Component c, const fmi2ValueReference vr[],size_t nvr, fmi2String value[]);

/*! \fn omsi_set_boolean
 *
 *  This function sets booleans of the model.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_set_boolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[]);

/*! \fn omsi_set_integer
 *
 *  This function sets integers of the model.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_set_integer(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[]);

/*! \fn omsi_set_real
 *
 *  This function sets reals of the model.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_set_real(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[]);

/*! \fn omsi_set_string
 *
 *  This function sets strings of the model.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_set_string(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2String value[]);

/*! \fn  omsi_set_time
 *
 *  This function sets the time of an instance.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_set_time(fmi2Component c, fmi2Real time);


//unsupported functions
/*! \fn omsi_get_fmu_state
 *
 *  This function is not supported.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_get_fmu_state(fmi2Component c, fmi2FMUstate* FMUstate);

/*! \fn omsi_set_fmu_state
 *
 *  This function is not supported.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_set_fmu_state(fmi2Component c, fmi2FMUstate FMUstate);

/*! \fn omsi_get_clock
 *
 *  This function sets the clock of the model.
 *
 *  \param [ref] [data]C
 */
fmi2Status omsi_get_clock(fmi2Component c,const fmi2Integer clockIndex[],size_t nClockIndex, fmi2Boolean tick[]);

/*! \fn omsi_get_interval
 *
 *  This function gets the interval of the model.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_get_interval(fmi2Component c, const fmi2Integer clockIndex[],size_t nClockIndex, fmi2Real interval[]);

/*! \fn omsi_set_clock
 *
 *  This function sets the clock of the model.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_set_clock(fmi2Component c, const fmi2Integer clockIndex[], size_t nClockIndex, const fmi2Boolean tick[],const fmi2Boolean subactive[]);

/*! \fn omsi_set_interval
 *
 *  This function sets the interval of the model.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_set_interval(fmi2Component c,const fmi2Integer clockIndex[],size_t nClockIndex, const fmi2Real interval[]);

#ifdef __cplusplus
}
#endif
#endif
