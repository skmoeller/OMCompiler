/**
 *  \file omsi.cpp
 *  \brief Brief
 */
/*
 * Implement OSU.
 *
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

//Cpp Simulation kernel includes
#include <Core/ModelicaDefine.h>
#include <Core/Modelica.h>
#include <Core/SimController/ISimController.h>
#include <Core/System/FactoryExport.h>
#include <Core/Utils/extension/logger.hpp>
#include <omsi_global_settings.h>
#include "omsi_fmi2_log.h"
#include "omsi_fmi2_wrapper.h"
#include <omsi_factory.h>


static fmi2String const _LogCategoryFMUNames[] = {
  "logEvents",
  "logSingularLinearSystems",
  "logNonlinearSystems",
  "logDynamicStateSelection",
  "logStatusWarning",
  "logStatusDiscard",
  "logStatusError",
  "logStatusFatal",
  "logStatusPending",
  "logFmi2Call"
};

OSI_callbacks InstantiateAndInitializeOSI_callbacks(){
		OSI_callbacks w;
		OSI_callbacks_init(&w);
		return w;
}

void OSI_callbacks_init(OSI_callbacks *OSI_callbacks_ptr){
		OSI_callbacks_ptr->setupExperiment = setupExperiment;
		OSI_callbacks_ptr->enterInitializationMode = enterInitializationMode;
		OSI_callbacks_ptr->exitInitializationMode = exitInitializationMode;
		OSI_callbacks_ptr->terminate = terminate;
		OSI_callbacks_ptr->terminate = terminatgetNominalsOfContinuousStatese;
		OSI_callbacks_ptr->getReal = getReal;
		OSI_callbacks_ptr->getInteger = getInteger;
		OSI_callbacks_ptr->getBoolean = getBoolean;
		OSI_callbacks_ptr->getString = getString;
		OSI_callbacks_ptr->getClock = getClock;
		OSI_callbacks_ptr->getInterval = getInterval;
		OSI_callbacks_ptr->setReal = setReal;
		OSI_callbacks_ptr->setInteger = setInteger;
		OSI_callbacks_ptr->setBoolean = setBoolean;
		OSI_callbacks_ptr->setString = setString;
		OSI_callbacks_ptr->setClock = setClock;
		OSI_callbacks_ptr->setInterval = setInterval;
		OSI_callbacks_ptr->newDiscreteStates = newDiscreteStates;
		OSI_callbacks_ptr->completedIntegratorStep = completedIntegratorStep;
		OSI_callbacks_ptr->setTime = setTime;
		OSI_callbacks_ptr->setContinuousStates = setContinuousStates;
		OSI_callbacks_ptr->getDerivatives = getDerivatives;
		OSI_callbacks_ptr->getEventIndicators = getEventIndicators;
		OSI_callbacks_ptr->getContinuousStates = getContinuousStates;
		OSI_callbacks_ptr->getNominalsOfContinuousStates = getNominalsOfContinuousStates;
	}


fmi2Status setupExperiment(fmi2Boolean toleranceDefined,
                                        fmi2Real tolerance,
                                        fmi2Real startTime,
                                        fmi2Boolean stopTimeDefined,
                                        fmi2Real stopTime)
{
  // ToDo: setup tolerance and stop time
  ModelInstance *comp = (ModelInstance *)c;
  if (invalidState(comp, "fmi2SetupExperiment", modelInstantiated, ~0))
    return fmi2Error;

  FILTERED_LOG(comp, fmi2OK, LOG_FMI2_CALL, "fmi2SetupExperiment: toleranceDefined=%d tolerance=%g startTime=%g stopTimeDefined=%d stopTime=%g", toleranceDefined, tolerance,
      startTime, stopTimeDefined, stopTime)

  comp->toleranceDefined = toleranceDefined;
  comp->tolerance = tolerance;
  comp->startTime = startTime;
  comp->stopTimeDefined = stopTimeDefined;
  comp->stopTime = stopTime;
  return fmi2OK;
}
  return setTime(startTime);
}

fmi2Status enterInitializationMode()
{
  _initialize_model->setInitial(true);
  _need_update = true;
  return fmi2OK;
}

fmi2Status exitInitializationMode()
{
  if (_need_update)
    updateModel();
  _event_model->saveAll();
  _initialize_model->setInitial(false);
  _time_event_model->initTimeEventData();
  return fmi2OK;
}

fmi2Status terminate()
{
  return fmi2OK;
}

fmi2Status reset()
{
  _initialize_model->initializeFreeVariables();
  return fmi2OK;
}

void updateModel()
{
  if (_initialize_model->initial()) {
    _initialize_model->initializeBoundVariables();
    _event_model->saveAll();
  }
  _continuous_model->evaluateAll();     // derivatives and algebraic variables
  _need_update = false;
}

fmi2Status setTime(fmi2Real time)
{
  if (_nclockTick > 0) {
    std::fill(_clockTick, _clockTick + _event_model->getDimClock(), false);
    std::fill(_clockSubactive, _clockSubactive + _event_model->getDimClock(), false);
    _nclockTick = 0;
  }
  _time_event_model->setTime(time);
  _need_update = true;
  return fmi2OK;
}

fmi2Status setContinuousStates(const fmi2Real states[], size_t nx)
{
  _continuous_model->setContinuousStates(states);
  _need_update = true;
  return fmi2OK;
}

fmi2Status getContinuousStates(fmi2Real states[], size_t nx)
{
  _continuous_model->getContinuousStates(states);
  return fmi2OK;
}

fmi2Status getDerivatives(fmi2Real derivatives[], size_t nx)
{
  if (_need_update)
    updateModel();
  _time_event_model->computeTimeEventConditions(_time_event_model->getTime());
  _continuous_model->getRHS(derivatives);
  return fmi2OK;
}

fmi2Status completedIntegratorStep(fmi2Boolean noSetFMUStatePriorToCurrentPoint,
                                                fmi2Boolean *enterEventMode,
                                                fmi2Boolean *terminateSimulation)
{
  _event_model->saveAll();
  *enterEventMode = fmi2False;
  *terminateSimulation = fmi2False;
  return fmi2OK;
}

// Functions for setting inputs and start values
fmi2Status setReal(const fmi2ValueReference vr[], size_t nvr,
                                const fmi2Real value[])
{

  double* realvars = _model->getSimObjects()->getSimVars(_model->getModelName())->getRealVarsVector();
  if(realvars)
  {
	  for (int i = 0; i < nvr; i++, vr++, value++) {
		// access variables and aliases in SimVars memory
		if (*vr < _continuous_model->getDimReal())
		  realvars[*vr] = *value;
		// convert negated aliases
		else switch (*vr) {
		  default:
			throw std::invalid_argument("setReal with wrong value reference " + omcpp::to_string(*vr));
			// TODO: insert return fmi2Error; in error cases
		}
	  }
	}
  else
	  throw std::invalid_argument("getReal with wrong real vars memory allocation");

  _need_update = true;
  return fmi2OK;
}

fmi2Status setInteger(const fmi2ValueReference vr[], size_t nvr,
                                   const fmi2Integer value[])
{
  int* intvars = _model->getSimObjects()->getSimVars(_model->getModelName())->getIntVarsVector();
  if(intvars)
  {
	   for (int i = 0; i < nvr; i++, vr++, value++) {
		// access variables and aliases in SimVars memory
		if (*vr < _continuous_model->getDimInteger())
		  intvars[*vr] = *value;
		// convert negated aliases
		else switch (*vr) {
		  default:
			throw std::invalid_argument("setInteger with wrong value reference " + omcpp::to_string(*vr));
		}
	  }
  }
  else
	  throw std::invalid_argument("setInt with wrong Integer vars memory allocation");
  _need_update = true;
  return fmi2OK;
}

fmi2Status setBoolean(const fmi2ValueReference vr[], size_t nvr,
                                   const fmi2Boolean value[])
{
   bool* boolvars = _model->getSimObjects()->getSimVars(_model->getModelName())->getBoolVarsVector();
  if(boolvars)
  {
	  for (int i = 0; i < nvr; i++, vr++, value++) {
		// access variables and aliases in SimVars memory
		if (*vr < _continuous_model->getDimBoolean())
		  boolvars[*vr] = *value;
		// convert negated aliases
		else switch (*vr) {
		  default:
			throw std::invalid_argument("setBoolean with wrong value reference " + omcpp::to_string(*vr));
		}
	  }
  }
  else
	  throw std::invalid_argument("setBool with wrong Boolean vars memory allocation");
  _need_update = true;
  return fmi2OK;
}

fmi2Status setString(const fmi2ValueReference vr[], size_t nvr,
                                  const fmi2String  value[])
{
  if (nvr > _string_buffer.size()) {
    FMU2_LOG(this, fmi2Error, logStatusError,
             "Attempt to set %d fmi2String; FMU only has %d",
             nvr, _string_buffer.size());
    return fmi2Error;
  }
  for (size_t i = 0; i < nvr; i++)
    _string_buffer[i] = string(value[i]); // convert to string




  string* stringvars = _model->getSimObjects()->getSimVars(_model->getModelName())->getStringVarsVector();

  if(stringvars)
  {

		  for (int i = 0; i < nvr; i++, vr++, value++) {
		// access variables and aliases in SimVars memory
		if (*vr < _continuous_model->getDimString())
		  stringvars[*vr] = _string_buffer[i];
		// convert negated aliases
		else switch (*vr) {
		  default:
			throw std::invalid_argument("setString with wrong value reference " + omcpp::to_string(*vr));
		}
	  }
  }


  _need_update = true;
  return fmi2OK;
}

fmi2Status setClock(const fmi2Integer clockIndex[],
                                 size_t nClockIndex, const fmi2Boolean tick[],
                                 const fmi2Boolean subactive[])
{
  for (int i = 0; i < nClockIndex; i++) {
    _clockTick[clockIndex[i] - 1] = tick[i];
    if (subactive != NULL)
      _clockSubactive[clockIndex[i] - 1] = subactive[i];
  }
  _nclockTick = 0;
  for (int i = 0; i < _event_model->getDimClock(); i++) {
    _nclockTick += _clockTick[i]? 1: 0;
  }
  _need_update = true;
  return fmi2OK;
}

fmi2Status setInterval(const fmi2Integer clockIndex[],
                                    size_t nClockIndex, const fmi2Real interval[])
{
  double *clockInterval = _event_model->clockInterval();
  for (int i = 0; i < nClockIndex; i++) {
    clockInterval[clockIndex[i] - 1] = interval[i];
    _event_model->setIntervalInTimEventData((clockIndex[i] - 1), interval[i]);
  }
  _need_update = true;
  return fmi2OK;
}

fmi2Status getEventIndicators(fmi2Real eventIndicators[], size_t ni)
{
  if (_need_update)
    updateModel();
  _event_model->getConditions(_conditions);
  _event_model->getZeroFunc(eventIndicators);
  for (int i = 0; i < ni; i++)
    if (!_conditions[i]) eventIndicators[i] = -eventIndicators[i];
  return fmi2OK;
}

// Functions for reading the values of variables
fmi2Status getReal(const fmi2ValueReference vr[], size_t nvr,
                                fmi2Real value[])
{
  if (_need_update)
    updateModel();

  double* realvars = _model->getSimObjects()->getSimVars(_model->getModelName())->getRealVarsVector();
  if(realvars)
  {
	   for (int i = 0; i < nvr; i++, vr++, value++) {
		// access variables and aliases in SimVars memory
		if (*vr < _continuous_model->getDimReal())
		  *value = realvars[*vr];
		// convert negated aliases
		else switch (*vr) {
		  default:
			throw std::invalid_argument("getReal with wrong value reference " + omcpp::to_string(*vr));
		}
	  }
  }
  else
	  throw std::invalid_argument("getReal with wrong real vars memory allocation");


  return fmi2OK;
}

fmi2Status getInteger(const fmi2ValueReference vr[], size_t nvr,
                                   fmi2Integer value[])
{
  if (_need_update)
    updateModel();
  int* intvars = _model->getSimObjects()->getSimVars(_model->getModelName())->getIntVarsVector();
  if(intvars)
  {
	  for (int i = 0; i < nvr; i++, vr++, value++) {
		// access variables and aliases in SimVars memory
		if (*vr < _continuous_model->getDimInteger())
		  *value = intvars[*vr];
		// convert negated aliases
		else switch (*vr) {
		  default:
			throw std::invalid_argument("getInteger with wrong value reference " + omcpp::to_string(*vr));
		}
	  }
  }
  else
	  throw std::invalid_argument("getInteger with wrong vars memory allocation");


  return fmi2OK;
}

fmi2Status getBoolean(const fmi2ValueReference vr[], size_t nvr,
                                   fmi2Boolean value[])
{
  if (_need_update)
    updateModel();
  bool* boolvars = _model->getSimObjects()->getSimVars(_model->getModelName())->getBoolVarsVector();
  if(boolvars)
  {
	  for (int i = 0; i < nvr; i++, vr++, value++) {
		// access variables and aliases in SimVars memory
		if (*vr < _continuous_model->getDimBoolean())
		  *value = boolvars[*vr];
		// convert negated aliases
		else switch (*vr) {
		  default:
			throw std::invalid_argument("getBoolean with wrong value reference " + omcpp::to_string(*vr));
		}
	  }
  }
  else
	  throw std::invalid_argument("getReal with wrong bool vars memory allocation");
  return fmi2OK;
}

fmi2Status getString(const fmi2ValueReference vr[], size_t nvr,
                                  fmi2String value[])
{
  if (nvr > _string_buffer.size()) {
    FMU2_LOG(this, fmi2Error, logStatusError,
             "Attempt to get %d fmi2String; FMU only has %d",
             nvr, _string_buffer.size());
    return fmi2Error;
  }
  if (_need_update)
    updateModel();

  string* stringvars = _model->getSimObjects()->getSimVars(_model->getModelName())->getStringVarsVector();

  if(stringvars)
  {

	  for (int i = 0; i < nvr; i++, vr++) {
		// access variables and aliases in SimVars memory
		if (*vr < _continuous_model->getDimString())
		 _string_buffer.push_back(stringvars[*vr]);
		// convert negated aliases
		else switch (*vr) {
		  default:
			throw std::invalid_argument("getString with wrong value reference " + omcpp::to_string(*vr));
		}
	  }
  }
  else
	 throw std::invalid_argument("getReal with wrong string vars memory allocation");

  for (size_t i = 0; i < nvr; i++)
    value[i] = _string_buffer[i].c_str(); // convert to fmi2String
  return fmi2OK;
}

fmi2Status getClock(const fmi2Integer clockIndex[],
                                 size_t nClockIndex, fmi2Boolean tick[])
{
  for (int i = 0; i < nClockIndex; i++) {
    tick[i] = _clockTick[clockIndex[i] - 1];
  }
  return fmi2OK;
}

fmi2Status getInterval(const fmi2Integer clockIndex[],
                                    size_t nClockIndex, fmi2Real interval[])
{
  double *clockInterval = _event_model->clockInterval();
  for (int i = 0; i < nClockIndex; i++) {
    interval[i] = clockInterval[clockIndex[i] - 1];
  }
  return fmi2OK;
}

fmi2Status newDiscreteStates(fmi2EventInfo *eventInfo)
{
  if (_need_update) {
    if (_nclockTick > 0)
      _event_model->setClock(_clockTick, _clockSubactive);
    updateModel();
    if (_nclockTick > 0) {
      // reset clocks
      std::fill(_clockTick, _clockTick + _event_model->getDimClock(), false);
      std::fill(_clockSubactive, _clockSubactive + _event_model->getDimClock(), false);
      _nclockTick = 0;
    }
  }
  // Check if an Zero Crossings happend

  _event_model->getZeroFunc(_zero_funcs);
  for (int i = 0; i < _event_model->getDimZeroFunc(); i++)
    _events[i] = _zero_funcs[i] >= 0;
  // Handle Zero Crossings if nessesary
  bool state_vars_reinitialized = _model->handleSystemEvents(_events);
  //time events
  eventInfo->nextEventTime = _time_event_model->computeNextTimeEvents(_time_event_model->getTime());
  if ((eventInfo->nextEventTime != 0.0) && (eventInfo->nextEventTime != std::numeric_limits<double>::max()))
    eventInfo->nextEventTimeDefined = fmi2True;
  else
    eventInfo->nextEventTimeDefined = fmi2False;
  // everything is done
  eventInfo->newDiscreteStatesNeeded = fmi2False;
  eventInfo->terminateSimulation = fmi2False;
  eventInfo->nominalsOfContinuousStatesChanged = state_vars_reinitialized;
  eventInfo->valuesOfContinuousStatesChanged = state_vars_reinitialized;
  return fmi2OK;
}

fmi2Status getNominalsOfContinuousStates(fmi2Real x_nominal[], size_t nx)
{
  for (int i = 0; i < nx; i++)
    x_nominal[i] = 1.0;  // TODO
  return fmi2OK;
}

/** @} */ // end of fmu2


