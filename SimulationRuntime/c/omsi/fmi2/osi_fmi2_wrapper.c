/* TODO: implement external functions in FMU wrapper for c++ target
 */

//OpenModelica Simulation Interface
#include "omsi.h"
#include "omsi/osu_me.h"
//FMI2 interface
#include "fmi2Functions.h"
#include "fmi2FunctionTypes.h"
#include "fmi2TypesPlatform.h"



FMI2_Export const char* fmi2GetTypesPlatform()
{
  return fmi2TypesPlatform;
}

FMI2_Export const char* fmi2GetVersion()
{
  return fmi2Version;
}

FMI2_Export fmi2Status fmi2SetDebugLogging(fmi2Component c,fmi2Boolean loggingOn,size_t  nCategories,const fmi2String categories[])
{
  return omsi_set_debug_logging(c, loggingOn, nCategories, categories);
}

FMI2_Export fmi2Component fmi2Instantiate(fmi2String instanceName,fmi2Type   fmuType,fmi2String fmuGUID,fmi2String fmuResourceLocation,const fmi2CallbackFunctions* functions, fmi2Boolean visible,fmi2Boolean loggingOn)
{
  return omsi_instantiate(instanceName, fmuType, fmuGUID, fmuResourceLocation, functions, visible, loggingOn);
}

FMI2_Export void fmi2FreeInstance(fmi2Component c)
{
  omsi_free_instance(c);
}

FMI2_Export fmi2Status fmi2SetupExperiment(fmi2Component c,fmi2Boolean   toleranceDefined,fmi2Real      tolerance, fmi2Real      startTime, fmi2Boolean   stopTimeDefined, fmi2Real      stopTime)
{
  return omsi_setup_experiment(c, toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime);
}

FMI2_Export fmi2Status fmi2EnterInitializationMode(fmi2Component c)
{
  return omsi_enter_initialization_mode(c);
}

FMI2_Export fmi2Status fmi2ExitInitializationMode(fmi2Component c)
{
  return omsi_exit_initialization_mode(c);
}

FMI2_Export fmi2Status fmi2Terminate(fmi2Component c)
{
  return omsi_terminate(c);
}

FMI2_Export fmi2Status fmi2Reset(fmi2Component c)
{
  return omsi_reset(c);
}

FMI2_Export fmi2Status fmi2GetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[])
{
  return omsi_get_real(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2GetInteger(fmi2Component c, const fmi2ValueReference vr[],size_t nvr, fmi2Integer value[])
{
  return omsi_get_integer(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2GetBoolean(fmi2Component c, const fmi2ValueReference vr[],size_t nvr, fmi2Boolean value[])
{
  return omsi_get_boolean(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2GetString(fmi2Component c, const fmi2ValueReference vr[],size_t nvr, fmi2String value[])
{
  return omsi_get_string(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2SetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[])
{
  return omsi_set_real(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2SetInteger(fmi2Component c, const fmi2ValueReference vr[],size_t nvr, const fmi2Integer value[])
{
  return omsi_set_integer(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2SetBoolean(fmi2Component c, const fmi2ValueReference vr[],size_t nvr, const fmi2Boolean value[])
{
  return omsi_set_boolean(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2SetString(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2String value[])
{
  return omsi_set_string(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2GetFMUstate(fmi2Component c, fmi2FMUstate* FMUstate)
{
  return omsi_get_fmu_state(c, FMUstate);
}

FMI2_Export fmi2Status fmi2SetFMUstate(fmi2Component c, fmi2FMUstate FMUstate)
{
  return omsi_set_fmu_state(c, FMUstate);
}

FMI2_Export fmi2Status fmi2FreeFMUstate(fmi2Component c, fmi2FMUstate* FMUstate)
{
  return omsi_free_fmu_state(c, FMUstate);
}

FMI2_Export fmi2Status fmi2SerializedFMUstateSize(fmi2Component c, fmi2FMUstate FMUstate,size_t* size)
{
  return omsi_serialized_fmu_state_size(c, FMUstate, size);
}

FMI2_Export fmi2Status fmi2SerializeFMUstate(fmi2Component c, fmi2FMUstate FMUstate,fmi2Byte serializedState[], size_t size)
{
  return omsi_serialize_fmu_state(c, FMUstate, serializedState, size);
}

FMI2_Export fmi2Status fmi2DeSerializeFMUstate(fmi2Component c, const fmi2Byte serializedState[],size_t size, fmi2FMUstate* FMUstate)
{
  return omsi_de_serialize_fmu_state(c, serializedState, size, FMUstate);
}

FMI2_Export fmi2Status fmi2GetDirectionalDerivative(fmi2Component c,const fmi2ValueReference vUnknown_ref[], size_t nUnknown,const fmi2ValueReference vKnown_ref[],   size_t nKnown, const fmi2Real dvKnown[], fmi2Real dvUnknown[])
{
  return omsi_get_directional_derivative(c, vUnknown_ref, nUnknown, vKnown_ref, nKnown, dvKnown, dvUnknown);
}

FMI2_Export fmi2Status fmi2EnterEventMode(fmi2Component c)
{
  return omsi_enter_event_mode(c);
}

FMI2_Export fmi2Status fmi2NewDiscreteStates(fmi2Component  c,fmi2EventInfo* fmiEventInfo)
{
  return omsi_new_discrete_state(c, fmiEventInfo);
}

FMI2_Export fmi2Status fmi2EnterContinuousTimeMode(fmi2Component c)
{
  return omsi_enter_continuous_time_mode(c);
}

FMI2_Export fmi2Status fmi2CompletedIntegratorStep(fmi2Component c,fmi2Boolean   noSetFMUStatePriorToCurrentPoint, fmi2Boolean*  enterEventMode, fmi2Boolean*   terminateSimulation)
{
  return omsi_completed_integrator_step(c, noSetFMUStatePriorToCurrentPoint,enterEventMode, terminateSimulation);
}

FMI2_Export fmi2Status fmi2SetTime(fmi2Component c, fmi2Real time)
{
  return omsi_set_time(c, time);
}

FMI2_Export fmi2Status fmi2SetContinuousStates(fmi2Component c, const fmi2Real x[],size_t nx)
{
  return omsi_set_continuous_states(c, x, nx);
}

FMI2_Export fmi2Status fmi2GetDerivatives(fmi2Component c, fmi2Real derivatives[], size_t nx)
{
  return omsi_get_derivatives(c, derivatives, nx);
}

FMI2_Export fmi2Status fmi2GetEventIndicators(fmi2Component c, fmi2Real eventIndicators[], size_t ni)
{
  return omsi_get_event_indicators(c, eventIndicators, ni);
}

FMI2_Export fmi2Status fmi2GetContinuousStates(fmi2Component c, fmi2Real x[],size_t nx)
{
  return omsi_get_continuous_states(c, x, nx);
}

FMI2_Export fmi2Status fmi2GetNominalsOfContinuousStates(fmi2Component c,fmi2Real x_nominal[],size_t nx)
{
  return omsi_get_nominals_of_continuous_states(c, x_nominal, nx);
}
