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

#include <omsi.h>
#include <omsi_callbacks.h>
#include <omsi_global.h>


#ifdef __cplusplus
extern "C" {
#endif

#if _MSC_VER
#define __attribute__(x)
#endif

/* used for filtered_log */
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

static omsi_string logCategoriesNames[] = {"logEvents", "logSingularLinearSystems", "logNonlinearSystems", "logDynamicStateSelection",
    "logStatusWarning", "logStatusDiscard", "logStatusError", "logStatusFatal", "logStatusPending", "logAll", "logFmi2Call"};

/*#define FILTERED_LOG(instance, status, categoryIndex, message, ...) if (isCategoryLogged(instance, categoryIndex)) \
    instance->fmiCallbackFunctions->logger(instance->fmiCallbackFunctions->componentEnvironment, instance->instanceName, status, \
        logCategoriesNames[categoryIndex], message, ##__VA_ARGS__);*/

#define LOG_FILTER(instance, categoryIndex, log_call) if(!global_callback) {        \
        printf("Error in LOG_FILTER: Global callback not set!\n"); fflush(stdout);  \
        abort();                                                                    \
      }                                                                             \
      if(isCategoryLogged(instance, categoryIndex)) {                               \
        log_call;                                                                   \
        fflush(stdout);                                                             \
      }                                                                             \

/* function prototypes */
omsi_bool isCategoryLogged(osu_t*       OSU,
                           omsi_int     categoryIndex);

omsi_string stateToString(osu_t* OSU);

omsi_bool invalidState(osu_t*       OSU,
                       omsi_string  function_name,
                       omsi_int     meStates,
                       omsi_int     csStates);

omsi_bool nullPointer(osu_t*        OSU,
                      omsi_string   function_name,
                      omsi_string   arg,
                      const void *  pointer);

omsi_bool vrOutOfRange(osu_t*               OSU,
                       omsi_string          function_name,
                       omsi_unsigned_int    vr,
                       omsi_int             end);

omsi_status unsupportedFunction(osu_t*      OSU,
                                omsi_string function_name,
                                omsi_int    statesExpected);

omsi_bool invalidNumber(osu_t*          OSU,
                        omsi_string     function_name,
                        omsi_string     arg,
                        omsi_int        n,
                        omsi_int        nExpected);

omsi_status omsi_set_debug_logging(osu_t*               OSU,
                                   omsi_bool            loggingOn,
                                   omsi_unsigned_int    nCategories,
                                   const omsi_string    categories[]);

void omsu_free_osu_data(omsi_t* omsi_data);

void omsu_free_model_data (model_data_t* model_data);

void omsu_free_sim_data (sim_data_t* sim_data);

void omsu_free_omsi_function(omsi_function_t* omsi_function);

void omsu_free_omsi_values(omsi_values* values);

#ifdef __cplusplus
}
#endif
#endif
