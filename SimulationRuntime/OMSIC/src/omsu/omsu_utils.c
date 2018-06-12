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
 * Interface (OMSI). These are helper functions used for the other omsu functions.
 */

#include "omsu/omsu_utils.h"

fmi2Boolean isCategoryLogged(fmi2Component c, int categoryIndex)
{
  osu_t* OSU = (osu_t *)c;
  if (categoryIndex < NUMBER_OF_CATEGORIES && (OSU->logCategories[categoryIndex] || OSU->logCategories[LOG_ALL])) {
    return fmi2True;
  }
  return fmi2False;
}

const char* stateToString(fmi2Component c)
 {
   osu_t* OSU = (osu_t *)c;
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

fmi2Boolean invalidState(fmi2Component c, const char *f, int meStates, int csStates) {   //ToDo: maybe omsi_data instead of fmi2Component data type
  osu_t* OSU = (osu_t *)c;
  if (!OSU)
    return fmi2True;

  int statesExpected;
  if (fmi2ModelExchange == OSU->type)
    statesExpected = meStates;
  else /* fmi2CoSimulation */
    statesExpected = csStates;

  if (!(OSU->state & statesExpected)) {
	OSU->state=statesExpected;
    FILTERED_LOG(OSU, fmi2Error, LOG_STATUSERROR, "%s: Illegal call sequence. %s is not allowed in %s state.", f, f, stateToString(OSU))
	OSU->state = modelError;
    return fmi2True;
  }

  return fmi2False;
}

fmi2Boolean nullPointer(fmi2Component c, const char *f, const char *arg, const void *p)
{
  osu_t* OSU = (osu_t *)c;
  if (!p) {
    OSU->state = modelError;
    FILTERED_LOG(OSU, fmi2Error, LOG_STATUSERROR, "%s: Invalid argument %s = NULL.", f, arg)
    return fmi2True;
  }
  return fmi2False;
}

fmi2Boolean vrOutOfRange(fmi2Component c, const char *f, fmi2ValueReference vr, int end)
{
  osu_t* OSU = (osu_t *)c;
  if ((int)vr >= end) {
    OSU->state = modelError;
    FILTERED_LOG(OSU, fmi2Error, LOG_STATUSERROR, "%s: Illegal value reference %u.", f, vr)
    return fmi2True;
  }
  return fmi2False;
}

fmi2Status unsupportedFunction(fmi2Component c, const char *fName, int statesExpected)
{
  osu_t* OSU = (osu_t *)c;
  //fmi2CallbackLogger log = OSU->osu_functions->logger; //TODO: makes sense?
  if (invalidState(c, fName, statesExpected, ~0))
    return fmi2Error;
  FILTERED_LOG(OSU, fmi2Error, LOG_STATUSERROR, "%s: Function not implemented.", fName)
  return fmi2Error;
}

fmi2Boolean invalidNumber(fmi2Component c, const char *f, const char *arg, int n, int nExpected)
{
  osu_t* OSU = (osu_t *)c;
  if (n != nExpected) {
    OSU->state = modelError;
    FILTERED_LOG(OSU, fmi2Error, LOG_STATUSERROR, "%s: Invalid argument %s = %d. Expected %d.", f, arg, n, nExpected)
    return fmi2True;
  }
  return fmi2False;
}

/*! \fn omsi_set_debug_logging
 *
 *  This function sets up debug logging.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_set_debug_logging(fmi2Component c, fmi2Boolean loggingOn, size_t nCategories, const fmi2String categories[])
{
  size_t i, j;
  osu_t* OSU = (osu_t *)c;
  OSU->loggingOn = loggingOn;

  for (i = 0; i < NUMBER_OF_CATEGORIES; i++) {
    OSU->logCategories[i] = fmi2False;
  }
  for (i = 0; i < nCategories; i++) {
    fmi2Boolean categoryFound = fmi2False;
    for (j = 0; j < NUMBER_OF_CATEGORIES; j++) {
      if (strcmp(logCategoriesNames[j], categories[i]) == 0) {
        OSU->logCategories[j] = loggingOn;
        categoryFound = fmi2True;
        break;
      }
    }
    if (!categoryFound) {
    	OSU->fmiCallbackFunctions->logger(OSU->fmiCallbackFunctions->componentEnvironment, OSU->instanceName, fmi2Warning, logCategoriesNames[j],
          "logging category '%s' is not supported by model", categories[i]);
    }
  }

  FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2SetDebugLogging")
  return fmi2OK;
}
