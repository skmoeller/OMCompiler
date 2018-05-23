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

#include "omsu_GettersAndSetters.h"

fmi2Status omsi_get_boolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[]){
	int i;
	osu_t *OSU = (osu_t *)c;
	if (invalidState(OSU, "fmi2GetBoolean", modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0))
		return fmi2Error;
	if (nvr > 0 && nullPointer(OSU, "fmi2GetBoolean", "vr[]", vr))
		return fmi2Error;
	if (nvr > 0 && nullPointer(OSU, "fmi2GetBoolean", "value[]", value))
		return fmi2Error;
	for (i = 0; i < nvr; i++){
		if (vrOutOfRange(OSU, "fmi2GetBoolean", vr[i], OSU->osu_data.model_data.n_bool_vars)) {
		    return fmi2Error;
		}
		value[i] = getBoolean(OSU, vr[i]);
		FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2GetBoolean: #b%u# = %s", vr[i], value[i]? "true" : "false")
	}
	return fmi2OK;
}

fmi2Status omsi_get_integer(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[]){
    int i;
    osu_t* OSU = (osu_t *)c;
    if (invalidState(OSU, "fmi2GetInteger", modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0))
      return fmi2Error;
    if (nvr > 0 && nullPointer(OSU, "fmi2GetInteger", "vr[]", vr))
      return fmi2Error;
    if (nvr > 0 && nullPointer(OSU, "fmi2GetInteger", "value[]", value))
      return fmi2Error;
    for (i = 0; i < nvr; i++) {
      if (vrOutOfRange(OSU, "fmi2GetInteger", vr[i], OSU->osu_data.model_data.n_int_vars)) {
        return fmi2Error;
      }
      value[i] = getInteger(OSU, vr[i]);
      FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2GetInteger: #i%u# = %d", vr[i], value[i])
    }
    return fmi2OK;
}

fmi2Status omsi_get_real(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[]){
    int i;
    osu_t *OSU = (osu_t *)c;
    if (invalidState(OSU, "fmi2GetReal", modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0))
      return fmi2Error;
    if (nvr > 0 && nullPointer(OSU, "fmi2GetReal", "vr[]", vr))
      return fmi2Error;
    if (nvr > 0 && nullPointer(OSU, "fmi2GetReal", "value[]", value))
      return fmi2Error;

    if (OSU->osu_data.model_data.n_real_vars > 0) {
		for (i = 0; i < nvr; i++) {
		  if (vrOutOfRange(OSU, "fmi2GetReal", vr[i], OSU->osu_data.model_data.n_real_vars)) {
		    return fmi2Error;
		  }
		  value[i] = getReal(OSU, vr[i]);
		  FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2GetReal: vr = %i, value = %f", vr[i], value[i])
		}
    }
    return fmi2OK;
}

fmi2Status omsi_get_string(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2String value[]){
	int i;
	osu_t *OSU = (osu_t *)c;
	if (invalidState(OSU, "fmi2GetString", modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0))
		return fmi2Error;
	if (nvr>0 && nullPointer(OSU, "fmi2GetString", "vr[]", vr))
		return fmi2Error;
	if (nvr>0 && nullPointer(OSU, "fmi2GetString", "value[]", value))
		return fmi2Error;
	for (i=0; i<nvr; i++) {
		if (vrOutOfRange(OSU, "fmi2GetString", vr[i], OSU->osu_data.model_data.n_string_vars))
			return fmi2Error;
		value[i] = getString(OSU, vr[i]); // to be implemented by the includer of this file
		FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2GetString: #s%u# = '%s'", vr[i], value[i])
	}
	return fmi2OK;
}

fmi2Status omsi_set_boolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[]){
	int i;
	osu_t *OSU = (osu_t *)c;
	int meStates = modelInstantiated|modelInitializationMode|modelEventMode;
	int csStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode;

	if (invalidState(OSU, "fmi2SetReal", meStates, csStates))
		return fmi2Error;
	if (nvr>0 && nullPointer(OSU, "fmi2SetBoolean", "vr[]", vr))
		return fmi2Error;
	if (nvr>0 && nullPointer(OSU, "fmi2SetBoolean", "value[]", value))
		return fmi2Error;
	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2SetBoolean: nvr = %d", nvr)

	for (i = 0; i < nvr; i++) {
		if (vrOutOfRange(OSU, "fmi2SetBoolean", vr[i], OSU->osu_data.model_data.n_bool_vars))
			return fmi2Error;
		FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2SetBoolean: #b%d# = %s", vr[i], value[i] ? "true" : "false")
		if (setBoolean(OSU, vr[i], value[i]) != fmi2OK) // to be implemented by the includer of this file
			return fmi2Error;
	}
	OSU->_need_update = 1;
	return fmi2OK;
}

fmi2Status omsi_set_integer(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[]){
	int i;
	osu_t *OSU = (osu_t *)c;
	int meStates = modelInstantiated|modelInitializationMode|modelEventMode;
	int csStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode;

	if (invalidState(OSU, "fmi2SetReal", meStates, csStates))
		return fmi2Error;
	if (nvr > 0 && nullPointer(OSU, "fmi2SetInteger", "vr[]", vr))
		return fmi2Error;
	if (nvr > 0 && nullPointer(OSU, "fmi2SetInteger", "value[]", value))
		return fmi2Error;
	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2SetInteger: nvr = %d", nvr)

	for (i = 0; i < nvr; i++) {
		if (vrOutOfRange(OSU, "fmi2SetInteger", vr[i], OSU->osu_data.model_data.n_int_vars))
			return fmi2Error;
		FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2SetInteger: #i%d# = %d", vr[i], value[i])
		if (setInteger(OSU, vr[i], value[i]) != fmi2OK) // to be implemented by the includer of this file
			return fmi2Error;
	}
	OSU->_need_update = 1;
	return fmi2OK;
}

fmi2Status omsi_set_real(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[]){
	int i;
	osu_t *OSU = (osu_t *)c;
	int meStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode;
	int csStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode;

	if (invalidState(OSU, "fmi2SetReal", meStates, csStates))
		return fmi2Error;
	if (nvr > 0 && nullPointer(OSU, "fmi2SetReal", "vr[]", vr))
		return fmi2Error;
	if (nvr > 0 && nullPointer(OSU, "fmi2SetReal", "value[]", value))
		return fmi2Error;
	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2SetReal: nvr = %d", nvr)
	// no check whether setting the value is allowed in the current state
	for (i = 0; i < nvr; i++) {
		if (vrOutOfRange(OSU, "fmi2SetReal", vr[i], OSU->osu_data.model_data.n_real_vars+OSU->osu_data.model_data.n_states))
			return fmi2Error;
		FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2SetReal: #r%d# = %.16g", vr[i], value[i])
		if (setReal(OSU, vr[i], value[i]) != fmi2OK) // to be implemented by the includer of this file
			return fmi2Error;
	}
	OSU->_need_update = 1;
	return fmi2OK;
}

fmi2Status omsi_set_string(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2String value[]){
	int i, n;
	osu_t *OSU = (osu_t *)c;
	int meStates = modelInstantiated|modelInitializationMode|modelEventMode;
	int csStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode;

	if (invalidState(OSU, "fmi2SetReal", meStates, csStates))
		return fmi2Error;
	if (nvr>0 && nullPointer(OSU, "fmi2SetString", "vr[]", vr))
		return fmi2Error;
	if (nvr>0 && nullPointer(OSU, "fmi2SetString", "value[]", value))
		return fmi2Error;
	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2SetString: nvr = %d", nvr)

	for (i = 0; i < nvr; i++) {
		if (vrOutOfRange(OSU, "fmi2SetString", vr[i], OSU->osu_data.model_data.n_string_vars))
			return fmi2Error;
		FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2SetString: #s%d# = '%s'", vr[i], value[i])

		if (setString(OSU, vr[i], value[i]) != fmi2OK) // to be implemented by the includer of this file
			return fmi2Error;
	}
	OSU->_need_update = 1;
	return fmi2OK;
}

fmi2Status omsi_set_time(fmi2Component c, fmi2Real time){
	osu_t *OSU = (osu_t *)c;
	if (invalidState(OSU, "fmi2SetTime", modelInstantiated|modelEventMode|modelContinuousTimeMode, ~0))
		return fmi2Error;
	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2SetTime: time=%.16g", time)
	OSU->old_data->localData[0]->timeValue = time;
	OSU->_need_update = 1;
	return fmi2OK;
}

fmi2Status omsi_get_fmu_state(fmi2Component c, fmi2FMUstate* FMUstate) {
	return unsupportedFunction(c, "fmi2GetFMUstate", modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError);
}


fmi2Status omsi_set_fmu_state(fmi2Component c, fmi2FMUstate FMUstate) {
	return unsupportedFunction(c, "fmi2SetFMUstate", modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError);
}


fmi2Status omsi_get_clock(fmi2Component c,const fmi2Integer clockIndex[],size_t nClockIndex, fmi2Boolean tick[])
{
	//TODO: implement
	return fmi2Error;
}

fmi2Status omsi_get_interval(fmi2Component c, const fmi2Integer clockIndex[],size_t nClockIndex, fmi2Real interval[])
{
	//TODO: implement
	return fmi2Error;
}

fmi2Status omsi_set_clock(fmi2Component c, const fmi2Integer clockIndex[], size_t nClockIndex, const fmi2Boolean tick[],const fmi2Boolean subactive[])
{
	//TODO: implement
	return fmi2Error;
}

fmi2Status omsi_set_interval(fmi2Component c,const fmi2Integer clockIndex[],size_t nClockIndex, const fmi2Real interval[])
{
	//TODO: implement
	return fmi2Error;
}
