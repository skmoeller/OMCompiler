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
 * This file defines functions for the FMI continuous simulation used via the OpenModelica
 * Simulation Interface (OMSI). These are the functions to evaluate the
 * model equations during continuous-time mode with OMSI.
 */

#ifndef OMSU_CONTINUOUSSIMULATION__H_
#define OMSU_CONTINUOUSSIMULATION__H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "omsu_helper.h"
#include "omsu_utils.h"
#include "omsu_me.h"
#include "fmi2/fmi2Functions.h"

/*! \fn  omsi_new_discrete_state
 *
 *  This function sets a the discrete state.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_new_discrete_state(fmi2Component  c, fmi2EventInfo* eventInfo);

/*! \fn  omsi_enter_continuous_time_mode
 *
 *  This function enters continuous time mode.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_enter_continuous_time_mode(fmi2Component c);

/*! \fn  omsi_set_continuous_states
 *
 *  This function sets continuous states of the model.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_set_continuous_states(fmi2Component c, const fmi2Real x[], size_t nx);

/*! \fn omsi_get_continuous_states
 *
 *  This function gets continuous states of the model.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_get_continuous_states(fmi2Component c, fmi2Real x[], size_t nx);

/*! \fn omsi_get_nominals_of_continuous_states
 *
 *  This function gets nominals of continuous states of the model.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_get_nominals_of_continuous_states(fmi2Component c,fmi2Real x_nominal[], size_t nx);

/*! \fn  omsi_completed_integrator_step
 *
 *  This function completes an integrator step
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_completed_integrator_step(fmi2Component c,
                                          fmi2Boolean   noSetFMUStatePriorToCurrentPoint,
                                          fmi2Boolean*  enterEventMode,
                                          fmi2Boolean*  terminateSimulation);
										  fmi2Status fmi2_get_fmu_state(fmi2Component c, fmi2FMUstate* FMUstate);

/*! \fn omsi_get_derivatives
*
*  This function obtains derivatives of the model.
*
*  \param [ref] [data]
*/
fmi2Status omsi_get_derivatives(fmi2Component c, fmi2Real derivatives[], size_t nx);

/*! \fn omsi_get_directional_derivative
 *
 *  This function gets the directional derivative.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_get_directional_derivative(fmi2Component c,
										  const fmi2ValueReference vUnknown_ref[], size_t nUnknown,
										  const fmi2ValueReference vKnown_ref[],   size_t nKnown,
										  const fmi2Real dvKnown[], fmi2Real dvUnknown[]);

#ifdef __cplusplus
}
#endif
#endif
