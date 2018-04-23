#ifndef OSU_INITIALIZATION__H_
#define OSU_INITIALIZATION__H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "osu_me.h"
#include "osu_utils.h"
#include "fmi2/fmi2Functions.h"

/*! \fn omsi_me_instantiate
 *
 *  This function instantiates the osu instance.
 *
 *  \param [ref] [data]
 */
fmi2Component omsi_me_instantiate(
                               fmi2String    instanceName,
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
