#ifndef OSU_UTILS__H_
#define OSU_UTILS__H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "osu_me.h"
#include "fmi2/fmi2Functions.h"
#include "simulation/solver/initialization/initialization.h"

//used for filtered_log
#define LOG_EVENTS                      0
#define LOG_SINGULARLINEARSYSTEMS       1
#define LOG_NONLINEARSYSTEMS            2
#define LOG_DYNAMICSTATESELECTION       3
#define LOG_STATUSWARNING               4
#define LOG_STATUSDISCARD               5
#define LOG_STATUSERROR                 6
#define LOG_STATUSFATAL                 7
#define LOG_STATUSPENDING               8
#define LOG_ALL                         9
#define LOG_FMI2_CALL                   10

static fmi2String logCategoriesNames[] = {"logEvents", "logSingularLinearSystems", "logNonlinearSystems", "logDynamicStateSelection",
    "logStatusWarning", "logStatusDiscard", "logStatusError", "logStatusFatal", "logStatusPending", "logAll", "logFmi2Call"};

#define FILTERED_LOG(instance, status, categoryIndex, message, ...) if (isCategoryLogged(instance, categoryIndex)) \
    instance->fmiCallbackFunctions->logger(instance->fmiCallbackFunctions->componentEnvironment, instance->instanceName, status, \
        logCategoriesNames[categoryIndex], message, ##__VA_ARGS__);

fmi2Boolean isCategoryLogged(fmi2Component c, int categoryIndex);
const char* stateToString(fmi2Component c);
fmi2Boolean invalidState(fmi2Component c, const char *f, int meStates, int csStates);
fmi2Boolean nullPointer(fmi2Component c, const char *f, const char *arg, const void *p);
fmi2Boolean vrOutOfRange(fmi2Component c, const char *f, fmi2ValueReference vr, int end);
fmi2Status unsupportedFunction(fmi2Component c, const char *fName, int statesExpected);
fmi2Boolean invalidNumber(fmi2Component c, const char *f, const char *arg, int n, int nExpected);

fmi2Status omsi_set_debug_logging(fmi2Component c,fmi2Boolean  loggingOn,size_t nCategories,const fmi2String categories[]);

#ifdef __cplusplus
}
#endif
#endif
