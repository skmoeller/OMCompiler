#ifdef __cplusplus
extern "C" {
#endif
#include "omsu_EventSimulation.h"





fmi2Status omsi_enter_event_mode(fmi2Component c){
	osu_t *OSU = (osu_t *)c;
	if (invalidState(OSU, "fmi2EnterEventMode", modelInitializationMode|modelContinuousTimeMode|modelEventMode, ~0))
		return fmi2Error;
	FILTERED_LOG(OSU, fmi2OK, LOG_EVENTS, "fmi2EnterEventMode")
	OSU->state = modelEventMode;
	return fmi2OK;
}


fmi2Status omsi_get_event_indicators(fmi2Component c, fmi2Real eventIndicators[], size_t nx)
{
	int i;
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
	for (i = 0; i < nx; i++) {
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


#ifdef __cplusplus
}
#endif
