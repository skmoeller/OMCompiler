#ifndef OSU_EVENTSIMULATION__H_
#define OSU_EVENTSIMULATION__H_
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "osu_me.h"
#include "osu_utils.h"
#include "fmi2/fmi2Functions.h"

/*! \fn omsi_enter_event_mode
 *
 *  This function updates the events.
 *
 *  \param [ref] [data]
 */
//fmi2Status fmi2EventUpdate(fmi2Component c, fmi2EventInfo* eventInfo);

/*! \fn omsi_enter_event_mode
 *
 *  This function entervs event mode.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_enter_event_mode(fmi2Component c);

/*! \fn omsi_get_event_indicators
 *
 *  This function gets the event indicators.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_get_event_indicators(fmi2Component c, fmi2Real eventIndicators[], size_t ni);

//fmi2Status fmi2EventUpdate(fmi2Component c, fmi2EventInfo* eventInfo);

#ifdef __cplusplus
}
#endif
#endif
