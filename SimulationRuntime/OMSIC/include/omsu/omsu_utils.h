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


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <omsu_common.h>


#ifdef __cplusplus
extern "C" {
#endif

/* used for filtered_log */
/*#define LOG_EVENTS                      0     ToDo: is already defined */
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

static omsi_string logCategoriesNames[] = {"logEvents", "logSingularLinearSystems", "logNonlinearSystems", "logDynamicStateSelection",
    "logStatusWarning", "logStatusDiscard", "logStatusError", "logStatusFatal", "logStatusPending", "logAll", "logFmi2Call"};

#define FILTERED_LOG(instance, status, categoryIndex, message, ...) if (isCategoryLogged(instance, categoryIndex)) \
    instance->fmiCallbackFunctions->logger(instance->fmiCallbackFunctions->componentEnvironment, instance->instanceName, status, \
        logCategoriesNames[categoryIndex], message, ##__VA_ARGS__);

/* function prototypes */
omsi_bool isCategoryLogged(void* c, omsi_int categoryIndex);
omsi_string stateToString(void* c);
omsi_bool invalidState(void* c, omsi_string f, omsi_int meStates, omsi_int csStates);
omsi_bool nullPointer(void* c, omsi_string f, omsi_string arg, const void *p);
omsi_bool vrOutOfRange(void* c, omsi_string f, omsi_unsigned_int vr, omsi_int end);
omsi_status unsupportedFunction(void* c, omsi_string fName, omsi_int statesExpected);
omsi_bool invalidNumber(void* c, omsi_string f, omsi_string arg, omsi_int n, omsi_int nExpected);

omsi_status omsi_set_debug_logging(void* c, omsi_bool loggingOn, omsi_unsigned_int nCategories, const omsi_string categories[]);


#ifdef __cplusplus
}
#endif
#endif
