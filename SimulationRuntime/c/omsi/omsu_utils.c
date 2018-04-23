#ifdef __cplusplus
extern "C" {
#endif
#include "osu_utils.h"

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

fmi2Boolean invalidState(fmi2Component c, const char *f, int meStates, int csStates)
{
  osu_t* OSU = (osu_t *)c;
  if (!OSU)
    return fmi2True;

  int statesExpected;
  if (fmi2ModelExchange == OSU->type)
    statesExpected = meStates;
  else /* fmi2CoSimulation */
    statesExpected = csStates;

  if (!(OSU->state & statesExpected))
  {
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
  if (vr >= end) {
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
  int i, j;
  osu_t* OSU = (osu_t *)c;
  OSU->loggingOn = loggingOn;

  for (j = 0; j < NUMBER_OF_CATEGORIES; j++) {
    OSU->logCategories[j] = fmi2False;
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
#ifdef __cplusplus
}
#endif
