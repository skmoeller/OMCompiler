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


#include <omsu_utils.h>

omsi_bool isCategoryLogged(void*        component,
                           omsi_int     categoryIndex) {

    osu_t* OSU = (osu_t *)component;
    if (categoryIndex < NUMBER_OF_CATEGORIES && (OSU->logCategories[categoryIndex] || OSU->logCategories[LOG_ALL])) {
        return omsi_true;
    }
    return omsi_false;
}

/*
 * Returns current state of component as string.
 */
omsi_string stateToString(void* component) {
    osu_t* OSU = (osu_t *)component;
    switch (OSU->state) {
        case modelInstantiated: return "Instantiated";
        case modelInitializationMode: return "Initialization Mode";
        case modelEventMode: return "Event Mode";
        case modelContinuousTimeMode: return "Continuous-Time Mode";
        case modelTerminated: return "Terminated";
        case modelError: return "Error";
        default: break;
    }
    return "Unknown";
}

/*
 * Checks if component environment is allowed to execute function_name in its
 * current state.
 */
omsi_bool invalidState(void*        component,      /* component environment */
                       omsi_string  function_name,  /* name of function */
                       omsi_int     meStates,       /* valid Model Exchange states for function */
                       omsi_int     csStates) {     /* valid Co-Simulation state for function */

    /* Variables */
    osu_t* OSU ;
    omsi_int statesExpected;

    OSU = (osu_t *)component;

    if (!OSU) {
        return omsi_true;
    }

    if (omsi_model_exchange == OSU->type) {
        statesExpected = meStates;
    }
    else { /* CoSimulation */
        statesExpected = csStates;
    }

    if (!(OSU->state & statesExpected)) {
        OSU->state = statesExpected;
        FILTERED_LOG(OSU, omsi_error, LOG_STATUSERROR, "%s: Illegal call sequence. %s is not allowed in %s state.", function_name, function_name, stateToString(OSU))
        OSU->state = modelError;
        return omsi_true;
    }

    return omsi_false;
}

omsi_bool nullPointer(void*         component,
                      omsi_string   function_name,
                      omsi_string   arg,
                      const void *  pointer) {

    osu_t* OSU = (osu_t *)component;
    if (!pointer) {
        OSU->state = modelError;
        FILTERED_LOG(OSU, omsi_error, LOG_STATUSERROR, "%s: Invalid argument %s = NULL.", function_name, arg)
        return omsi_true;
    }
    return omsi_false;
}

omsi_bool vrOutOfRange(void*                component,
                       omsi_string          function_name,
                       omsi_unsigned_int    vr,
                       omsi_int             end) {

    osu_t* OSU = (osu_t *)component;
    if ((omsi_int)vr >= end) {
        OSU->state = modelError;
        FILTERED_LOG(OSU, omsi_error, LOG_STATUSERROR, "%s: Illegal value reference %u.", function_name, vr)
        return omsi_true;
    }
    return omsi_false;
}

omsi_status unsupportedFunction(void*       component,
                                omsi_string function_name,
                                omsi_int    statesExpected) {

    osu_t* OSU = (osu_t *)component;
    if (invalidState(component, function_name, statesExpected, ~0)) {
        return omsi_error;
    }
    FILTERED_LOG(OSU, omsi_error, LOG_STATUSERROR, "%s: Function not implemented.", function_name)
    return omsi_error;
}

omsi_bool invalidNumber(void*           component,
                        omsi_string     function_name,
                        omsi_string     arg,
                        omsi_int        n,
                        omsi_int        nExpected) {

    osu_t* OSU = (osu_t *)component;
    if (n != nExpected) {
        OSU->state = modelError;
        FILTERED_LOG(OSU, omsi_error, LOG_STATUSERROR, "%s: Invalid argument %s = %d. Expected %d.", function_name, arg, n, nExpected)
        return omsi_true;
    }
    return omsi_false;
}

/*! \fn omsi_set_debug_logging
 *
 *  This function sets up debug logging.
 *
 *  \param [ref] [data]
 */
omsi_status omsi_set_debug_logging(void*                component,
                                   omsi_bool            loggingOn,
                                   omsi_unsigned_int    nCategories,
                                   const omsi_string    categories[]) {

    omsi_unsigned_int i, j;
    osu_t* OSU = (osu_t *)component;
    OSU->loggingOn = loggingOn;

    for (i = 0; i < NUMBER_OF_CATEGORIES; i++) {
        OSU->logCategories[i] = omsi_false;
    }
    for (i = 0; i < nCategories; i++) {
        omsi_bool categoryFound = omsi_false;
        for (j = 0; j < NUMBER_OF_CATEGORIES; j++) {
            if (strcmp(logCategoriesNames[j], categories[i]) == 0) {
                OSU->logCategories[j] = loggingOn;
                categoryFound = omsi_true;
                break;
            }
        }
        if (!categoryFound) {
            OSU->fmiCallbackFunctions->logger(OSU->fmiCallbackFunctions->componentEnvironment, OSU->instanceName, omsi_warning, logCategoriesNames[j],
                    "logging category '%s' is not supported by model", categories[i]);
        }
    }

    FILTERED_LOG(OSU, omsi_ok, LOG_FMI2_CALL, "fmi2SetDebugLogging")
    return omsi_ok;
}
