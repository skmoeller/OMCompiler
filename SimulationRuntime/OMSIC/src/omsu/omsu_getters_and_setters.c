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

#include <omsu_getters_and_setters.h>

#define UNUSED(x) (void)(x)     /* ToDo: delete later */

/*
 * ============================================================================
 * Getters
 * ============================================================================
 */

omsi_status omsi_get_boolean(osu_t*                     OSU,
                             const omsi_unsigned_int    vr[],
                             omsi_unsigned_int          nvr,
                             omsi_bool                  value[]){

    /* Variables */
    omsi_unsigned_int i;

    /* Check inputs and if call is valid*/
    if (invalidState(OSU, "fmi2GetBoolean", modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0)) {
        return omsi_error;
    }
    if (nvr > 0 && nullPointer(OSU, "fmi2GetBoolean", "vr[]", vr)) {
        return omsi_error;
    }
    if (nvr > 0 && nullPointer(OSU, "fmi2GetBoolean", "value[]", value)) {
        return omsi_error;
    }

    /* Get bool values */
    for (i = 0; i < nvr; i++){
        if (vrOutOfRange(OSU, "fmi2GetBoolean", vr[i], OSU->osu_data->sim_data->model_vars_and_params->n_bools)) {
            return omsi_error;
        }
        value[i] = getBoolean(OSU, vr[i]);
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
                "fmi2GetBoolean: #b%u# = %s", vr[i], value[i]? "true" : "false");
    }
    return omsi_ok;
}

omsi_status omsi_get_integer(osu_t*                     OSU,
                             const omsi_unsigned_int    vr[],
                             omsi_unsigned_int          nvr,
                             omsi_int                   value[]){

    /* Variables */
    omsi_unsigned_int i;

    /* Check inputs and if call is valid*/
    if (invalidState(OSU, "fmi2GetInteger", modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0)) {
      return omsi_error;
    }
    if (nvr > 0 && nullPointer(OSU, "fmi2GetInteger", "vr[]", vr)) {
      return omsi_error;
    }
    if (nvr > 0 && nullPointer(OSU, "fmi2GetInteger", "value[]", value)) {
      return omsi_error;
    }

    /* Get integers */
    for (i = 0; i < nvr; i++) {
      if (vrOutOfRange(OSU, "fmi2GetInteger", vr[i], OSU->osu_data->sim_data->model_vars_and_params->n_ints)) {
        return omsi_error;
      }
      value[i] = getInteger(OSU, vr[i]);
      filtered_base_logger(global_logCategories, log_all, omsi_ok,
              "fmi2GetInteger: #i%u# = %d", vr[i], value[i]);
    }
    return omsi_ok;
}

omsi_status omsi_get_real(osu_t*                    OSU,
                          const omsi_unsigned_int   vr[],
                          omsi_unsigned_int         nvr,
                          omsi_real                 value[]){

    /* Variables */
    omsi_unsigned_int i;

    /* Check inputs and if call is valid*/
    if (invalidState(OSU, "fmi2GetReal", modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0)) {
      return omsi_error;
    }
    if (nvr > 0 && nullPointer(OSU, "fmi2GetReal", "vr[]", vr)) {
      return omsi_error;
    }
    if (nvr > 0 && nullPointer(OSU, "fmi2GetReal", "value[]", value)) {
      return omsi_error;
    }

    /* Get reals */
    for (i = 0; i < nvr; i++) {
        if (vrOutOfRange(OSU, "fmi2GetReal", vr[i], OSU->osu_data->sim_data->model_vars_and_params->n_reals)) {
            return omsi_error;
        }
        value[i] = getReal(OSU, vr[i]);
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
                "fmi2GetReal: vr = %i, value = %f", vr[i], value[i]);
    }
    return omsi_ok;
}

omsi_status omsi_get_string(osu_t*                  OSU,
                            const omsi_unsigned_int vr[],
                            omsi_unsigned_int       nvr,
                            omsi_string             value[]){

    /* Variables */
    omsi_unsigned_int i;

    if (invalidState(OSU, "fmi2GetString", modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0))
        return omsi_error;
    if (nvr>0 && nullPointer(OSU, "fmi2GetString", "vr[]", vr))
        return omsi_error;
    if (nvr>0 && nullPointer(OSU, "fmi2GetString", "value[]", value))
        return omsi_error;
    for (i = 0; i < nvr; i++) {
        if (vrOutOfRange(OSU, "fmi2GetString", vr[i], OSU->osu_data->sim_data->model_vars_and_params->n_strings))
            return omsi_error;
        value[i] = getString(OSU, vr[i]); /* to be implemented by the includer of this file */
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
                "fmi2GetString: #s%u# = '%s'", vr[i], value[i]);
    }
    return omsi_ok;
}

omsi_status omsi_get_fmu_state(osu_t*        OSU,
                               void **      FMUstate) {

    /* TODO: implement */
    UNUSED(OSU); UNUSED (FMUstate);
    return omsi_error;
}

omsi_status omsi_get_clock(osu_t*               OSU,
                           const omsi_int       clockIndex[],
                           omsi_unsigned_int    nClockIndex,
                           omsi_bool            tick[]) {

    /* TODO: implement */
    UNUSED(OSU); UNUSED(clockIndex); UNUSED(nClockIndex); UNUSED(tick);
    return omsi_error;
}

omsi_status omsi_get_interval(osu_t*            OSU,
                              const omsi_int    clockIndex[],
                              omsi_unsigned_int nClockIndex,
                              omsi_real         interval[]) {

    /* TODO: implement */
    UNUSED(OSU); UNUSED(clockIndex); UNUSED(nClockIndex); UNUSED(interval);
    return omsi_error;
}

/*
 * ============================================================================
 * Setters
 * ============================================================================
 */

omsi_status omsi_set_boolean(osu_t*                     OSU,
                             const omsi_unsigned_int    vr[],
                             omsi_unsigned_int          nvr,
                             const omsi_bool            value[]) {

    /* Variables */
    omsi_unsigned_int i;
    omsi_int meStates, csStates;

    meStates = modelInstantiated|modelInitializationMode|modelEventMode;
    csStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode;

    if (invalidState(OSU, "fmi2SetReal", meStates, csStates))
        return omsi_error;
    if (nvr>0 && nullPointer(OSU, "fmi2SetBoolean", "vr[]", vr))
        return omsi_error;
    if (nvr>0 && nullPointer(OSU, "fmi2SetBoolean", "value[]", value))
        return omsi_error;

    filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2SetBoolean: nvr = %d", nvr);

    for (i = 0; i < nvr; i++) {
        if (vrOutOfRange(OSU, "fmi2SetBoolean", vr[i], OSU->osu_data->model_data->n_bool_vars))
            return omsi_error;
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
                "fmi2SetBoolean: #b%d# = %s", vr[i], value[i] ? "true" : "false");
        if (setBoolean(OSU, vr[i], value[i]) != omsi_ok) /* to be implemented by the includer of this file */
            return omsi_error;
    }
    OSU->_need_update = 1;
    return omsi_ok;
}

omsi_status omsi_set_integer(osu_t*                     OSU,
                             const omsi_unsigned_int    vr[],
                             omsi_unsigned_int                     nvr,
                             const omsi_int             value[]) {

    /* Variables */
    omsi_unsigned_int i;
    omsi_int meStates, csStates;

    meStates = modelInstantiated|modelInitializationMode|modelEventMode;
    csStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode;

    if (invalidState(OSU, "fmi2SetReal", meStates, csStates))
        return omsi_error;
    if (nvr > 0 && nullPointer(OSU, "fmi2SetInteger", "vr[]", vr))
        return omsi_error;
    if (nvr > 0 && nullPointer(OSU, "fmi2SetInteger", "value[]", value))
        return omsi_error;

    filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2SetInteger: nvr = %d", nvr);

    for (i = 0; i < nvr; i++) {
        if (vrOutOfRange(OSU, "fmi2SetInteger", vr[i], OSU->osu_data->model_data->n_int_vars)) {
            return omsi_error;
        }
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
                "fmi2SetInteger: #i%d# = %d", vr[i], value[i]);
        if (setInteger(OSU, vr[i], value[i]) != omsi_ok) {
            return omsi_error;
        }
    }
    OSU->_need_update = 1;
    return omsi_ok;
}

omsi_status omsi_set_real(osu_t*                    OSU,
                          const omsi_unsigned_int   vr[],
                          omsi_unsigned_int                    nvr,
                          const omsi_real           value[]) {

    /* Variables */
    omsi_unsigned_int i;
    omsi_int meStates, csStates;

    meStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode;
    csStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode;

    if (invalidState(OSU, "fmi2SetReal", meStates, csStates))
        return omsi_error;
    if (nvr > 0 && nullPointer(OSU, "fmi2SetReal", "vr[]", vr))
        return omsi_error;
    if (nvr > 0 && nullPointer(OSU, "fmi2SetReal", "value[]", value))
        return omsi_error;

    filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2SetReal: nvr = %d", nvr);

    for (i = 0; i < nvr; i++) {
        if (vrOutOfRange(OSU, "fmi2SetReal", vr[i], OSU->osu_data->model_data->n_real_vars+OSU->osu_data->model_data->n_states)) {
            return omsi_error;
        }
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
                "fmi2SetReal: #r%d# = %.16g", vr[i], value[i]);
        if (setReal(OSU, vr[i], value[i]) != omsi_ok) {
            return omsi_error;
        }
    }
    OSU->_need_update = 1;
    return omsi_ok;
}

omsi_status omsi_set_string(osu_t*                  OSU,
                            const omsi_unsigned_int vr[],
                            omsi_unsigned_int                  nvr,
                            const omsi_string       value[]) {

    /* Variables */
    omsi_unsigned_int i;
    omsi_int meStates, csStates;

    meStates = modelInstantiated|modelInitializationMode|modelEventMode;
    csStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode;

    if (invalidState(OSU, "fmi2SetReal", meStates, csStates))
        return omsi_error;
    if (nvr>0 && nullPointer(OSU, "fmi2SetString", "vr[]", vr))
        return omsi_error;
    if (nvr>0 && nullPointer(OSU, "fmi2SetString", "value[]", value))
        return omsi_error;

    filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2SetString: nvr = %d", nvr);

    for (i = 0; i < nvr; i++) {
        if (vrOutOfRange(OSU, "fmi2SetString", vr[i], OSU->osu_data->model_data->n_string_vars)) {
            return omsi_error;
        }
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
                "fmi2SetString: #s%d# = '%s'", vr[i], value[i]);

        if (setString(OSU, vr[i], value[i]) != omsi_ok) {
            return omsi_error;
        }
    }
    OSU->_need_update = 1;
    return omsi_ok;
}

omsi_status omsi_set_time(osu_t*    OSU,
                          omsi_real time) {

    if (invalidState(OSU, "fmi2SetTime", modelInstantiated|modelEventMode|modelContinuousTimeMode, ~0))
        return omsi_error;

    filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2SetTime: time=%.16g", time);

    OSU->osu_data->sim_data->model_vars_and_params->time_value = time;
    OSU->_need_update = 1;
    return omsi_ok;
}

omsi_status omsi_set_fmu_state(osu_t*   OSU,
                               void *   FMUstate) {

    /* TODO: implement */
    UNUSED(OSU); UNUSED(FMUstate);
    return omsi_error;
}

omsi_status omsi_set_clock(osu_t*               OSU,
                           const omsi_int       clockIndex[],
                           omsi_unsigned_int    nClockIndex,
                           const omsi_bool      tick[],
                           const omsi_bool      subactive[]) {

    /* TODO: implement */
    UNUSED(OSU); UNUSED(clockIndex); UNUSED(nClockIndex); UNUSED(tick); UNUSED(subactive);
    return omsi_error;
}

omsi_status omsi_set_interval(osu_t*            OSU,
                              const omsi_int    clockIndex[],
                              omsi_unsigned_int nClockIndex,
                              const omsi_real   interval[]) {

    /* TODO: implement */
    UNUSED(OSU); UNUSED(clockIndex); UNUSED(nClockIndex); UNUSED(interval);
    return omsi_error;
}




/*
 * ============================================================================
 * Helper functions for getters and setters
 * ============================================================================
 */

/* What happens for alias variables for getters and setters? */

/*
 * Gets real number of struct OSU with value reference vr.
 */
omsi_real getReal (osu_t*                   OSU,
                   const omsi_unsigned_int  vr) {

    omsi_real output = (omsi_real) OSU->osu_data->sim_data->model_vars_and_params->reals[vr];
    return output;
}


/*
 * Sets real number of struct OSU for index reference vr with value
 */
omsi_status setReal(osu_t*                  OSU,
                    const omsi_unsigned_int vr,
                    const omsi_real         value) {

    OSU->osu_data->sim_data->model_vars_and_params->reals[vr] = (omsi_real) value;
    return omsi_ok;
}


/*
 * Gets integer number of struct OSU with value reference vr
 */
omsi_int getInteger (osu_t*                     OSU,
                     const omsi_unsigned_int    vr) {

    /* Variables */
    omsi_unsigned_int index;
    omsi_int output;

    index = vr - OSU->osu_data->sim_data->model_vars_and_params->n_reals;
    output = (omsi_int) OSU->osu_data->sim_data->model_vars_and_params->ints[index];
    return output;
}

/*
 * Sets integer number of struct OSU for index reference vr with value
 */
omsi_status setInteger(osu_t*                   OSU,
                       const omsi_unsigned_int  vr,
                       const omsi_int           value) {

    /* Variables */
    omsi_unsigned_int index;

    index = vr - OSU->osu_data->sim_data->model_vars_and_params->n_reals;
    OSU->osu_data->sim_data->model_vars_and_params->ints[index] = (omsi_int) value;
    return omsi_ok;
}


/*
 * gets boolean variable of struct OSU with value reference vr
 */
omsi_bool getBoolean (osu_t*                    OSU,
                      const omsi_unsigned_int   vr) {

    /* Variables */
    omsi_unsigned_int index;
    omsi_bool output;

    index = vr - OSU->osu_data->sim_data->model_vars_and_params->n_reals
               - OSU->osu_data->sim_data->model_vars_and_params->n_ints;
    output = (omsi_bool) OSU->osu_data->sim_data->model_vars_and_params->bools[index];
    return output;
}

/*
 * sets boolean variable of struct OSU for index reference vr with value
 */
omsi_status setBoolean(osu_t*                   OSU,
                       const omsi_unsigned_int  vr,
                       const omsi_bool          value) {

    /* Variables */
    omsi_unsigned_int index;

    index = vr - OSU->osu_data->sim_data->model_vars_and_params->n_reals
               - OSU->osu_data->sim_data->model_vars_and_params->n_ints;
    OSU->osu_data->sim_data->model_vars_and_params->bools[index] = (omsi_bool) value;
    return omsi_ok;
}


/*
 * gets stringe of struct OSU with value reference vr
 * ToDO: implement string in sim_data_t
 */
omsi_string getString (osu_t*                   OSU,
                       const omsi_unsigned_int  vr) {

    UNUSED(OSU); UNUSED(vr);
    return "ERROR";
}

/*
 * sets string of struct OSU for index reference vr with value
 *  * ToDO: implement string in sim_data_t
 */
omsi_status setString(osu_t*                    OSU,
                      const omsi_unsigned_int   vr,
                      const omsi_string         value) {

    UNUSED(OSU); UNUSED(vr); UNUSED(value);
    return omsi_error;
}
