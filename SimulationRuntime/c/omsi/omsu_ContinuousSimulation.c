#ifdef __cplusplus
extern "C" {
#endif
#include "omsu_ContinuousSimulation.h"


fmi2Status omsi_new_discrete_state(fmi2Component  c, fmi2EventInfo* eventInfo){
	osu_t *OSU = (osu_t *)c;
	double nextSampleEvent = 0;
	fmi2Status returnValue = fmi2OK;

	if (invalidState(OSU, "fmi2NewDiscreteStates", modelEventMode, ~0))
		return fmi2Error;
	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2NewDiscreteStates")

	eventInfo->newDiscreteStatesNeeded = fmi2False;
	eventInfo->terminateSimulation = fmi2False;
	eventInfo->nominalsOfContinuousStatesChanged = fmi2False;
	eventInfo->valuesOfContinuousStatesChanged = fmi2False;
	eventInfo->nextEventTimeDefined = fmi2False;
	eventInfo->nextEventTime = 0;

	returnValue = fmi2EventUpdate(OSU, eventInfo);
	return returnValue;
}


fmi2Status omsi_enter_continuous_time_mode(fmi2Component c){
	osu_t *OSU = (osu_t *)c;
	if (invalidState(OSU, "fmi2EnterContinuousTimeMode", modelEventMode, ~0))
		return fmi2Error;
	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL,"fmi2EnterContinuousTimeMode")
	OSU->state = modelContinuousTimeMode;
	return fmi2OK;
}


fmi2Status omsi_set_continuous_states(fmi2Component c, const fmi2Real x[], size_t nx){
	osu_t *OSU = (osu_t *)c;
	int i;

	/* According to FMI RC2 specification fmi2SetContinuousStates should only be allowed in Continuous-Time Mode.
	* The following code is done only to make the FMUs compatible with Dymola because Dymola is trying to call fmi2SetContinuousStates after fmi2EnterInitializationMode.
	*/
	if (invalidState(OSU, "fmi2SetContinuousStates", modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode, ~0))
	/*if (invalidState(OSU, "fmi2SetContinuousStates", modelContinuousTimeMode))*/
		return fmi2Error;
	if (invalidNumber(OSU, "fmi2SetContinuousStates", "nx", nx, OSU->osu_data.model_data.n_states))
		return fmi2Error;
	if (nullPointer(OSU, "fmi2SetContinuousStates", "x[]", x))
		return fmi2Error;

	for (i = 0; i < nx; i++) {
		fmi2ValueReference vr = OSU->vrStates[i];
		FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2SetContinuousStates: #r%d# = %.16g", vr, x[i])
		if (setReal(OSU, vr, x[i]) != fmi2OK) {
		  return fmi2Error;
                }
	}
	OSU->_need_update = 1;

	return fmi2OK;
}


fmi2Status omsi_get_continuous_states(fmi2Component c, fmi2Real x[], size_t nx) {
	int i;
	osu_t* OSU = (osu_t *)c;
	if (invalidState(OSU, "fmi2GetContinuousStates", modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0))
		return fmi2Error;
	if (invalidNumber(OSU, "fmi2GetContinuousStates", "nx", nx, OSU->osu_data.model_data.n_states))
		return fmi2Error;
	if (nullPointer(OSU, "fmi2GetContinuousStates", "states[]", x))
		return fmi2Error;

	for (i = 0; i < nx; i++) {
		fmi2ValueReference vr = OSU->vrStates[i];
		x[i] = getReal(OSU, vr); // to be implemented by the includer of this file
		FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2GetContinuousStates: #r%u# = %.16g", vr, x[i])
		}

	return fmi2OK;
}


fmi2Status omsi_get_nominals_of_continuous_states(fmi2Component c, fmi2Real x_nominal[], size_t nx){
	int i;
	osu_t* OSU = (osu_t *)c;
	if (invalidState(OSU, "fmi2GetNominalsOfContinuousStates", modelInstantiated|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0))
		return fmi2Error;
	if (invalidNumber(OSU, "fmi2GetNominalsOfContinuousStates", "nx", nx, OSU->osu_data.model_data.n_states))
		return fmi2Error;
	if (nullPointer(OSU, "fmi2GetNominalsOfContinuousStates", "x_nominal[]", x_nominal))
		return fmi2Error;
	x_nominal[0] = 1;
	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2GetNominalsOfContinuousStates: x_nominal[0..%d] = 1.0", nx-1)
	for (i = 0; i < nx; i++)
		x_nominal[i] = 1;
	return fmi2OK;
}


fmi2Status omsi_completed_integrator_step(fmi2Component c, fmi2Boolean noSetFMUStatePriorToCurrentPoint, fmi2Boolean* enterEventMode, fmi2Boolean* terminateSimulation){

	osu_t *OSU = (osu_t *)c;
	threadData_t *threadData = OSU->threadData;
	if (invalidState(OSU, "fmi2OSUletedIntegratorStep", modelContinuousTimeMode, ~0))
		return fmi2Error;
	if (nullPointer(OSU, "fmi2OSUletedIntegratorStep", "enterEventMode", enterEventMode))
		return fmi2Error;
	if (nullPointer(OSU, "fmi2OSUletedIntegratorStep", "terminateSimulation", terminateSimulation))
		return fmi2Error;
	FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL,"fmi2OSUletedIntegratorStep")

	/* try */
	MMC_TRY_INTERNAL(simulationJumpBuffer)

	OSU->osu_functions->functionAlgebraics(OSU->old_data, threadData);
	OSU->osu_functions->output_function(OSU->old_data, threadData);
	OSU->osu_functions->function_storeDelayed(OSU->old_data, threadData);
	storePreValues(OSU->old_data);
	*enterEventMode = fmi2False;
	*terminateSimulation = fmi2False;
	/******** check state selection ********/
	#if !defined(OMC_NO_STATESELECTION)
	if (stateSelection(OSU->old_data, threadData, 1, 0)){
		/* if new set is calculated reinit the solver */
		*enterEventMode = fmi2True;
		FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL,"fmi2CompletedIntegratorStep: Need to iterate state values changed!")
	}
	#endif
	/* TODO: fix the extrapolation in non-linear system
	*       then we can stop to save all variables in
	*       in the whole ringbuffer
	*/
	overwriteOldSimulationData(OSU->old_data);
	return fmi2OK;
	/* catch */
	MMC_CATCH_INTERNAL(simulationJumpBuffer)

	FILTERED_LOG(OSU, fmi2Error, LOG_FMI2_CALL, "fmi2CompletedIntegratorStep: terminated by an assertion.")
	return fmi2Error;
}


fmi2Status omsi_get_derivatives(fmi2Component c, fmi2Real derivatives[], size_t nx)
{
  osu_t* OSU = (osu_t *)c;
  threadData_t *threadData = OSU->threadData;

  if (invalidState(OSU, "fmi2GetDerivatives", modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0))
    return fmi2Error;
  if (invalidNumber(OSU, "fmi2GetDerivatives", "nx", nx, OSU->osu_data.model_data.n_states))
    return fmi2Error;
  if (nullPointer(OSU, "fmi2GetDerivatives", "derivatives[]", derivatives))
    return fmi2Error;


  /* try */
  MMC_TRY_INTERNAL(simulationJumpBuffer)

    if (OSU->_need_update){
      OSU->osu_functions->functionODE(OSU->old_data, threadData);
      // runtime function -> overwriteOldSimulationData(OSU->fmuData);

      OSU->_need_update = 0;
    }

    for (int i = 0; i < nx; i++) {
      fmi2ValueReference vr = OSU->vrStatesDerivatives[i];
      derivatives[i] = getReal(OSU, vr); // to be implemented by the includer of this file
      FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2GetDerivatives: #r%d# = %.16g", vr, derivatives[i])
    }

    return fmi2OK;
  /* catch */
  MMC_CATCH_INTERNAL(simulationJumpBuffer)

  FILTERED_LOG(OSU, fmi2Error, LOG_FMI2_CALL, "fmi2GetDerivatives: terminated by an assertion.")
  return fmi2Error;
}


fmi2Status omsi_get_directional_derivative(fmi2Component c,
                const fmi2ValueReference vUnknown_ref[], size_t nUnknown,
                const fmi2ValueReference vKnown_ref[],   size_t nKnown,
                const fmi2Real dvKnown[], fmi2Real dvUnknown[]){
  int i,j;
  osu_t *OSU = (osu_t *)c;
  if (invalidState(OSU, "fmi2GetDirectionalDerivative", modelInstantiated|modelEventMode|modelContinuousTimeMode, ~0))
    return fmi2Error;
  FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2GetDirectionalDerivative")
  if (!OSU->_has_jacobian)
    return unsupportedFunction(c, "fmi2GetDirectionalDerivative", modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError);
  /***************************************/
  // This code assumes that the FMU variables are always sorted,
  // states first and then derivatives.
  // This is true for the actual OMC FMUs.
  // Anyway we'll check that the references are in the valid range
  for (i = 0; i < nUnknown; i++) {
    if (vUnknown_ref[i]>=OSU->osu_data.model_data.n_states)
        // We are only computing the A part of the Jacobian for now
        // so unknowns can only be states
        return fmi2Error;
  }
  for (i = 0; i < nKnown; i++) {
    if (vKnown_ref[i]>=2*OSU->osu_data.model_data.n_states) {
        // We are only computing the A part of the Jacobian for now
        // so knowns can only be states derivatives
        return fmi2Error;
    }
  }
  OSU->osu_functions->functionFMIJacobian(OSU->old_data, OSU->threadData, vUnknown_ref, nUnknown, vKnown_ref, nKnown, (double*)dvKnown, dvUnknown);

  /***************************************/
  return fmi2OK;
}


#ifdef __cplusplus
}
#endif
