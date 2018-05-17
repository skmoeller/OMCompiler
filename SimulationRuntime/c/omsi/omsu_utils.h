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

#ifndef OMSU_UTILS__H_
#define OMSU_UTILS__H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "omsu_common.h"
//#include "fmi2/fmi2Functions.h"
#include "../simulation/solver/initialization/initialization.h"

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

/* function prototypes */
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
