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
 * This file defines functions for the FMI event mode used via the OpenModelica
 * Simulation Interface (OMSI). These are the core functions to evaluate the
 * model equations with OMSI.
 */

#include <omsu_event_simulation.h>

#define UNUSED(x) (void)(x)     /* ToDo: delete later */

/*
 * The model enters Event Mode from the Continuous-Time Mode and discrete-time
 * equations may become active (and relations are not “frozen”).
 */
omsi_status omsi_enter_event_mode(osu_t* OSU) {

    if (invalidState(OSU, "fmi2EnterEventMode", modelInitializationMode|modelContinuousTimeMode|modelEventMode, ~0)) {
        return omsi_error;
    }

    /* Log call */
    filtered_base_logger(global_logCategories, log_fmi2_call, omsi_ok,
            "fmi2EnterEventMode");

    OSU->state = modelEventMode;
    return omsi_ok;
}


/*
 * Computes event indicators at current time instant and for the current states.
 */
omsi_status omsi_get_event_indicators(osu_t*            OSU,
                                      omsi_real         eventIndicators[],
                                      omsi_unsigned_int ni) {               /* number of event indicators */

    /* threadData_t *threadData = OSU->threadData; */
    /* Variables */

    UNUSED(eventIndicators);

    /* According to FMI RC2 specification fmi2GetEventIndicators should only be
     * allowed in Event Mode, Continuous-Time Mode & terminated. The following code
     * is done only to make the FMUs compatible with Dymola because Dymola is
     * trying to call fmi2GetEventIndicators after fmi2EnterInitializationMode.
     */
    if (invalidState(OSU, "fmi2GetEventIndicators", modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0)) {
        return omsi_error;
    }
    /* Check if number of event indicators ni is valid */
    if (invalidNumber(OSU, "fmi2GetEventIndicators", "ni", ni, OSU->osu_data->model_data->n_zerocrossings)) {
        return omsi_error;
    }

    /* Log call */
    filtered_base_logger(global_logCategories, log_fmi2_call, omsi_ok,
            "fmi2GetEventIndicators");

    /* If there are no event indicator there is nothing to do */
    if (OSU->osu_data->model_data->n_zerocrossings == 0) {
        return omsi_ok;
    }

    /* ToDo: try */
    /* MMC_TRY_INTERNAL(simulationJumpBuffer)*/

    /* Evaluate needed equations ToDo: is in FMI Standard but not really needed...
    if (OSU->_need_update) {
        OSU->osu_data->sim_data->simulation->evaluate (OSU->osu_data->sim_data->simulation, OSU->osu_data->sim_data->model_vars_and_params, NULL);
        OSU->_need_update = omsi_false;
    }*/

#if 0       /* ToDo: Add stuff */
    /* Get event indicators */
    OSU->osu_functions->function_ZeroCrossings(OSU->osu_data->sim_data->simulation, OSU->osu_data->sim_data->model_vars_and_params, NULL);
    for (i = 0; i < ni; i++) {
        eventIndicators[i] = OSU->osu_data->sim_data->zerocrossings_vars[i];
        LOG_FILTER(OSU, LOG_ALL,
            global_callback->logger(OSU, global_instance_name, omsi_ok, logCategoriesNames[LOG_ALL],
            "fmi2GetEventIndicators: z%d = %.16g", i, eventIndicators[i]))
    }
#endif
    return omsi_ok;

    /* ToDo: catch */
    /* MMC_CATCH_INTERNAL(simulationJumpBuffer)

    FILTERED_LOG(OSU, omsi_ok, LOG_FMI2_CALL, "error", "fmi2GetEventIndicators: terminated by an assertion.");
    return omsi_error;*/
}



omsi_status omsi_event_update(osu_t*              OSU,
                              omsi_event_info*    eventInfo) {

    /* Variables */
    omsi_status status;
    /* threadData_t *threadData = OSU->threadData; */

    if (nullPointer(OSU, "fmi2EventUpdate", "eventInfo", eventInfo)) {
        return omsi_error;
    }

    /* Log function call */
    filtered_base_logger(global_logCategories, log_fmi2_call, omsi_ok,
            "fmi2EventUpdate: Start Event Update! Next Sample Event %u",
            eventInfo->nextEventTime);

    eventInfo->valuesOfContinuousStatesChanged = omsi_false;

    /* ToDo: try */
    /* MMC_TRY_INTERNAL(simulationJumpBuffer)*/

#if 0
#if !defined(OMC_NO_STATESELECTION)
    if (stateSelection(OSU->old_data, OSU->threadData, 1, 1)) {
        LOG_FILTER(OSU, LOG_FMI2_CALL,
            global_callback->logger(OSU, global_instance_name, omsi_ok, logCategoriesNames[LOG_ALL],
            "fmi2EventUpdate: Need to iterate state values changed!"))
        /* if new set is calculated reinit the solver */
        eventInfo->valuesOfContinuousStatesChanged = omsi_true;
    }
#endif
#endif

    omsu_storePreValues(OSU->osu_data);

    /* ToDo: Add sample events */
#if 0
    /* activate sample event */
    for (i = 0; i < OSU->old_data->modelData->nSamples; ++i) {
        if (OSU->old_data->simulationInfo->nextSampleTimes[i]
                <= OSU->old_data->localData[0]->timeValue) {
            OSU->old_data->simulationInfo->samples[i] = 1;
            //infoStreamPrint(LOG_EVENTS, 0, "[%ld] sample(%g, %g)", OSU->old_data->modelData->samplesInfo[i].index, OSU->old_data->modelData->samplesInfo[i].start, OSU->old_data->modelData->samplesInfo[i].interval);
        }
    }
#endif

    /*evaluate functionDAE */
    status = OSU->osu_data->sim_data->simulation->evaluate(OSU->osu_data->sim_data->simulation, OSU->osu_data->sim_data->simulation->function_vars, NULL);
    if (status != omsi_ok) {
        return status;
    }

#if 0
    /* deactivate sample events */
    for (i = 0; i < OSU->old_data->modelData->nSamples; ++i) {
        if (OSU->old_data->simulationInfo->samples[i]) {
            OSU->old_data->simulationInfo->samples[i] = 0;
            OSU->old_data->simulationInfo->nextSampleTimes[i] +=
                    OSU->old_data->modelData->samplesInfo[i].interval;
        }
    }

    for (i = 0; i < OSU->old_data->modelData->nSamples; ++i)
        if ((i == 0)
                || (OSU->old_data->simulationInfo->nextSampleTimes[i]
                        < OSU->old_data->simulationInfo->nextSampleEvent))
            OSU->old_data->simulationInfo->nextSampleEvent =
                    OSU->old_data->simulationInfo->nextSampleTimes[i];
#endif

    /* ToDo: check for discrete changes */
#if 0
    if (omsu_discrete_changes(OSU->osu_data, thread_data) || eventInfo->valuesOfContinuousStatesChanged) {
        /* ? */
    }

    //if(OSU->old_data->callback->checkForDiscreteChanges(OSU->old_data, OSU->threadData) || OSU->old_data->simulationInfo->needToIterate || checkRelations(OSU->old_data) || eventInfo->valuesOfContinuousStatesChanged)
    if (OSU->osu_functions->checkForDiscreteChanges(OSU->old_data,
            OSU->threadData) || OSU->old_data->simulationInfo->needToIterate
            || checkRelations(OSU->old_data)
            || eventInfo->valuesOfContinuousStatesChanged) {
        FILTERED_LOG(OSU, omsi_ok, LOG_FMI2_CALL,
                "fmi2EventUpdate: Need to iterate(discrete changes)!")
        eventInfo->newDiscreteStatesNeeded = omsi_true;
        eventInfo->nominalsOfContinuousStatesChanged = omsi_false;
        eventInfo->valuesOfContinuousStatesChanged = omsi_true;
        eventInfo->terminateSimulation = omsi_false;
    } else {
        eventInfo->newDiscreteStatesNeeded = omsi_false;
        eventInfo->nominalsOfContinuousStatesChanged = omsi_false;
        eventInfo->terminateSimulation = omsi_false;
    }
#endif

    filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2EventUpdate: newDiscreteStatesNeeded %s",
            eventInfo->newDiscreteStatesNeeded ? "true" : "false");

#if 0
    /* due to an event overwrite old values */
    overwriteOldSimulationData(OSU->old_data);

    /* TODO: check the event iteration for relation
     * in fmi2 import and export. This is an workaround,
     * since the iteration seem not starting.
     */
    omsu_storePreValues(OSU->osu_data);
    updateRelationsPre(OSU->old_data);

    //Get Next Event Time
    omsi_real nextSampleEvent = 0;
    nextSampleEvent = getNextSampleTimeFMU(OSU->old_data);
    if (nextSampleEvent == -1) {
        eventInfo->nextEventTimeDefined = omsi_false;
    } else {
        eventInfo->nextEventTimeDefined = omsi_true;
        eventInfo->nextEventTime = nextSampleEvent;
    }
#endif

    filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2EventUpdate: Checked for Sample Events! Next Sample Event %u",
            eventInfo->nextEventTime);

    return omsi_ok;


    /* ToDo: catch */
    /* MMC_CATCH_INTERNAL(simulationJumpBuffer)

    FILTERED_LOG(OSU, omsi_error, LOG_FMI2_CALL,
            "fmi2EventUpdate: terminated by an assertion.")
    OSU->_need_update = omsi_true;
    return omsi_error; */
}
