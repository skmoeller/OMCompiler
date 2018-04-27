#ifndef OSU_CONTINUOUSSIMULATION__H_
#define OSU_CONTINUOUSSIMULATION__H_
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "omsu_me.h"
#include "omsu_utils.h"
#include "fmi2/fmi2Functions.h"

/*! \fn  omsi_new_discrete_state
 *
 *  This function sets a the discrete state.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_new_discrete_state(fmi2Component  c, fmi2EventInfo* fmiEventInfo);

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
