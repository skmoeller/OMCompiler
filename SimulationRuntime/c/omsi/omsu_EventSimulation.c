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

#include "omsu_EventSimulation.h"


fmi2Status omsi_enter_event_mode(fmi2Component c) {
	osu_t *OSU = (osu_t *)c;
	if (invalidState(OSU, "fmi2EnterEventMode", modelInitializationMode|modelContinuousTimeMode|modelEventMode, ~0))
		return fmi2Error;
	FILTERED_LOG(OSU, fmi2OK, LOG_EVENTS, "fmi2EnterEventMode")
	OSU->state = modelEventMode;
	return fmi2OK;
}

fmi2Status omsi_get_event_indicators(fmi2Component c, fmi2Real eventIndicators[], size_t nx) {
	osu_t* OSU = (osu_t *)c;
	threadData_t *threadData = OSU->threadData;

	/* According to FMI RC2 specification fmi2GetEventIndicators should only be allowed in Event Mode, Continuous-Time Mode & terminated.
	* The following code is done only to make the FMUs compatible with Dymola because Dymola is trying to call fmi2GetEventIndicators after fmi2EnterInitializationMode.
	*/
	if (invalidState(OSU, "fmi2GetEventIndicators", modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0))
	/*if (invalidState(OSU, "fmi2GetEventIndicators", modelEventMode|modelContinuousTimeMode|modelTerminated|modelError))*/
		return fmi2Error;
	//if (invalidNumber(OSU, "fmi2GetEventIndicators", "nx", nx, NUMBER_OF_EVENT_INDICATORS)) TODO: event indicators?
	//	return fmi2Error;

	/* try */
	MMC_TRY_INTERNAL(simulationJumpBuffer)

//	#if NUMBER_OF_EVENT_INDICATORS>0

	/* eval needed equations*/
	if (OSU->_need_update){
		OSU->osu_functions->functionODE(OSU->old_data, threadData);
		OSU->_need_update = 0;
	}
	OSU->osu_functions->function_ZeroCrossings(OSU->old_data, threadData, OSU->old_data->simulationInfo->zeroCrossings);
	for (size_t i = 0; i < nx; i++) {
		eventIndicators[i] = OSU->old_data->simulationInfo->zeroCrossings[i];
		FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2GetEventIndicators: z%d = %.16g", i, eventIndicators[i])
	}
//	#endif
	return fmi2OK;

	/* catch */
	MMC_CATCH_INTERNAL(simulationJumpBuffer)

	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "error", "fmi2GetEventIndicators: terminated by an assertion.");
	return fmi2Error;
}
