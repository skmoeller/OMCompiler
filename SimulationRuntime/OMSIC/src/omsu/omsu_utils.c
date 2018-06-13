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

omsi_bool isCategoryLogged(void* c, omsi_int categoryIndex)
{
  osu_t* OSU = (osu_t *)c;
  if (categoryIndex < NUMBER_OF_CATEGORIES && (OSU->logCategories[categoryIndex] || OSU->logCategories[LOG_ALL])) {
    return true;
  }
  return false;
}

omsi_string stateToString(void* c)
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

omsi_bool invalidState(void* c, omsi_string f, omsi_int meStates, omsi_int csStates) {   //ToDo: maybe omsi_data instead of fmi2Component data type
  osu_t* OSU = (osu_t *)c;
  if (!OSU)
    return true;

  omsi_int statesExpected;
  if (omsi_model_exchange == OSU->type)
    statesExpected = meStates;
  else /* CoSimulation */
    statesExpected = csStates;

  if (!(OSU->state & statesExpected)) {
	OSU->state=statesExpected;
    FILTERED_LOG(OSU, omsi_error, LOG_STATUSERROR, "%s: Illegal call sequence. %s is not allowed in %s state.", f, f, stateToString(OSU))
	OSU->state = modelError;
    return true;
  }

  return false;
}

omsi_bool nullPointer(void* c, omsi_string f, omsi_string arg, const void *p)
{
  osu_t* OSU = (osu_t *)c;
  if (!p) {
    OSU->state = modelError;
    FILTERED_LOG(OSU, omsi_error, LOG_STATUSERROR, "%s: Invalid argument %s = NULL.", f, arg)
    return true;
  }
  return false;
}

omsi_bool vrOutOfRange(void* c, omsi_string f, omsi_unsigned_int vr, omsi_int end)
{
  osu_t* OSU = (osu_t *)c;
  if ((omsi_int)vr >= end) {
    OSU->state = modelError;
    FILTERED_LOG(OSU, omsi_error, LOG_STATUSERROR, "%s: Illegal value reference %u.", f, vr)
    return true;
  }
  return false;
}

omsi_status unsupportedFunction(void* c, omsi_string fName, omsi_int statesExpected)
{
  osu_t* OSU = (osu_t *)c;
  //fmi2CallbackLogger log = OSU->osu_functions->logger; //TODO: makes sense?
  if (invalidState(c, fName, statesExpected, ~0))
    return omsi_error;
  FILTERED_LOG(OSU, omsi_error, LOG_STATUSERROR, "%s: Function not implemented.", fName)
  return omsi_error;
}

omsi_bool invalidNumber(void* c, omsi_string f, omsi_string arg, omsi_int n, omsi_int nExpected)
{
  osu_t* OSU = (osu_t *)c;
  if (n != nExpected) {
    OSU->state = modelError;
    FILTERED_LOG(OSU, omsi_error, LOG_STATUSERROR, "%s: Invalid argument %s = %d. Expected %d.", f, arg, n, nExpected)
    return true;
  }
  return false;
}

/*! \fn omsi_set_debug_logging
 *
 *  This function sets up debug logging.
 *
 *  \param [ref] [data]
 */
omsi_status omsi_set_debug_logging(void* c, omsi_bool loggingOn, omsi_unsigned_int nCategories, omsi_string categories[])
{
  omsi_unsigned_int i, j;
  osu_t* OSU = (osu_t *)c;
  OSU->loggingOn = loggingOn;

  for (i = 0; i < NUMBER_OF_CATEGORIES; i++) {
    OSU->logCategories[i] = false;
  }
  for (i = 0; i < nCategories; i++) {
    omsi_bool categoryFound = false;
    for (j = 0; j < NUMBER_OF_CATEGORIES; j++) {
      if (strcmp(logCategoriesNames[j], categories[i]) == 0) {
        OSU->logCategories[j] = loggingOn;
        categoryFound = true;
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
