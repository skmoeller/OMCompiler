/** @addtogroup fmu2
 *
 *  @{
 */

#ifndef OIS_FMI2_OSI_callbacks_H
#define OIS_FMI2_OSI_callbacks_H



/*
 * Wrap a Modelica System of the Cpp runtime for FMI 2.0.
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

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>


#include "omsi_fmi2_settings.h"

#include "fmi2Functions.h"




	// Enter and exit initialization mode, terminate and reset
	fmi2Status setupExperiment(fmi2Boolean toleranceDefined,fmi2Real tolerance,
						   fmi2Real startTime, fmi2Boolean stopTimeDefined,fmi2Real stopTime);
	fmi2Status enterInitializationMode();
	fmi2Status exitInitializationMode();
	fmi2Status terminate      ();
	fmi2Status reset          ();

	// Getting and setting variable values
	fmi2Status getReal   (const fmi2ValueReference vr[], size_t nvr, fmi2Real value[]);
	fmi2Status getInteger(const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[]);
	fmi2Status getBoolean(const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[]);
	fmi2Status getString (const fmi2ValueReference vr[], size_t nvr, fmi2String value[]);
	fmi2Status getClock  (const fmi2Integer clockIndex[], size_t nClockIndex, fmi2Boolean tick[]);
	fmi2Status getInterval(const fmi2Integer clockIndex[], size_t nClockIndex, fmi2Real interval[]);
	fmi2Status setReal   (const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[]);
	fmi2Status setInteger(const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[]);
	fmi2Status setBoolean(const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[]);
	fmi2Status setString (const fmi2ValueReference vr[], size_t nvr, const fmi2String  value[]);
	fmi2Status setClock  (const fmi2Integer clockIndex[], size_t nClockIndex, const fmi2Boolean tick[], const fmi2Boolean subactive[]);
	fmi2Status setInterval(const fmi2Integer clockIndex[], size_t nClockIndex, const fmi2Real interval[]);

	// Enter and exit the different modes for Model Exchange
	fmi2Status newDiscreteStates      (fmi2EventInfo *eventInfo);
	fmi2Status completedIntegratorStep(fmi2Boolean noSetFMUStatePriorToCurrentPoint,fmi2Boolean *enterEventMode,
										 fmi2Boolean *terminateSimulation);

	// Providing independent variables and re-initialization of caching
	fmi2Status setTime                (fmi2Real time);
	fmi2Status setContinuousStates    (const fmi2Real x[], size_t nx);

	// Evaluation of the model equations
	fmi2Status getDerivatives     (fmi2Real derivatives[], size_t nx);
	fmi2Status getEventIndicators (fmi2Real eventIndicators[], size_t ni);
	fmi2Status getContinuousStates(fmi2Real states[], size_t nx);
	fmi2Status getNominalsOfContinuousStates(fmi2Real x_nominal[], size_t nx);


	//function pointer of OSU callback functions
	typedef fmi2Status (*setupExperiment_ptr)(fmi2Boolean toleranceDefined,fmi2Real tolerance,
			   fmi2Real startTime, fmi2Boolean stopTimeDefined,fmi2Real stopTime);
	typedef fmi2Status (*enterInitializationMode_ptr)();
	typedef fmi2Status (*exitInitializationMode_ptr)();
	typedef fmi2Status (*terminate_ptr)();
	typedef fmi2Status (*reset_ptr)();
	typedef fmi2Status (*getReal_ptr)(const fmi2ValueReference vr[], size_t nvr, fmi2Real value[]);
	typedef fmi2Status (*getInteger_ptr)(const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[]);
	typedef fmi2Status (*getBoolean_ptr)(const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[]);
	typedef fmi2Status (*getString_ptr)(const fmi2ValueReference vr[], size_t nvr, fmi2String value[]);
	typedef fmi2Status (*getClock_ptr)(const fmi2Integer clockIndex[], size_t nClockIndex, fmi2Boolean tick[]);
	typedef fmi2Status (*getInterval_ptr)(const fmi2Integer clockIndex[], size_t nClockIndex, fmi2Real interval[]);
	typedef fmi2Status (*setReal_ptr)(const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[]);
	typedef fmi2Status (*setInteger_ptr)(const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[]);
	typedef fmi2Status (*setBoolean_ptr)(const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[]);
	typedef fmi2Status (*setString_ptr)(const fmi2ValueReference vr[], size_t nvr, const fmi2String  value[]);
	typedef fmi2Status (*setClock_ptr)(const fmi2Integer clockIndex[],
						size_t nClockIndex, const fmi2Boolean tick[], const fmi2Boolean subactive[]);
	typedef fmi2Status (*setInterval_ptr)(const fmi2Integer clockIndex[], size_t nClockIndex, const fmi2Real interval[]);
	typedef fmi2Status (*newDiscreteStates_ptr)(fmi2EventInfo *eventInfo);
	typedef fmi2Status (*completedIntegratorStep_ptr)(fmi2Boolean noSetFMUStatePriorToCurrentPoint,
						fmi2Boolean *enterEventMode,fmi2Boolean *terminateSimulation);
	typedef fmi2Status (*setTime_ptr)(fmi2Real time);
	typedef fmi2Status (*setContinuousStates_ptr)(const fmi2Real x[], size_t nx);
	typedef fmi2Status (*getDerivatives_ptr)(fmi2Real derivatives[], size_t nx);
	typedef fmi2Status (*getEventIndicators_ptr)(fmi2Real eventIndicators[], size_t ni);
	typedef fmi2Status (*getContinuousStates_ptr)();
	typedef fmi2Status (*getNominalsOfContinuousStates_ptr)(fmi2Real x_nominal[], size_t nx);


	typedef struct OSI_callbacks{
		setupExperiment_ptr setupExperiment;
		enterInitializationMode_ptr enterInitializationMode;
		exitInitializationMode_ptr exitInitializationMode;
		terminate_ptr terminate;
		reset_ptr terminateterminate;
		getReal_ptr getReal;
		getInteger_ptr getInteger;
		getBoolean_ptr getBoolean;
		getString_ptr getString;
		getClock_ptr getClock;
		getInterval_ptr getInterval;
		setReal_ptr setReal;
		setInteger_ptr setInteger;
		setBoolean_ptr setBoolean;
		setString_ptr setString;
		setClock_ptr setClock;
		setInterval_ptr setInterval;
		newDiscreteStates_ptr newDiscreteStates;
		completedIntegratorStep_ptr completedIntegratorStep;
		setTime_ptr setTime;
		setContinuousStates_ptr setContinuousStates;
		getDerivatives_ptr getDerivatives;
		getEventIndicators_ptr getEventIndicators;
		getContinuousStates_ptr getContinuousStates;
		getNominalsOfContinuousStates_ptr getNominalsOfContinuousStates;
	} OSI_callbacks;

	//setting default values for structs is not possible, so it must get initialized
	void OSI_callbacks_init(OSI_callbacks *OSI_callbacks_ptr);
	//create and initialize OSI_callbacks, use it like this: > OSI_callbacks [name] = CreateAndInitializeOSI_callbacks;
	OSI_callbacks InstantiateAndInitializeOSI_callbacks();


	/** @} */ // end of fmu2


#endif
