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

#ifndef _OMSIC_H
#define _OMSIC_H


#include <omsi.h>
#include <omsi_callbacks.h>

/* OMSIBase includes */
#include <omsi_global.h>
#include <omsi_utils.h>


#ifdef __cplusplus
extern "C" {
#endif


/*
 * ============================================================================
 * Central structure containing all informations
 * ============================================================================
 */
typedef struct osu_t {
    /* open modelica simulation interface data */
    omsi_t*             osu_data;           /* pointer to omsi_data struct, contains all data for simulation */
    omsi_template_callback_functions_t*   osu_functions;

    omsi_bool           _need_update;           /* Bool if model needs update */
    omsi_bool           _has_jacobian;          /* Bool if model has jocobian matrix. Defaults to omsi_false */
    ModelState          state;
    omsi_bool*          logCategories;          /* Pointer to osu_data->logCategories array */
    omsi_bool*          loggingOn;              /* Pointer to osu_data->loggingOn */
    omsi_char*          GUID;
    omsi_string         instanceName;
    omsi_event_info     eventInfo;
    omsi_bool           toleranceDefined;   /* ToDo: delete up to stopTime, redundant information. Already in osu_data->model_info */
    omsi_real           tolerance;
    omsi_real           startTime;
    omsi_bool           stopTimeDefined;
    omsi_real           stopTime;
    omsu_type           type;
    omsi_unsigned_int*  vrStates;               /* Array of value references of states */
    omsi_unsigned_int*  vrStatesDerivatives;    /* Array of value references of state derivatives */

    const omsi_callback_functions*  fmiCallbackFunctions;
} osu_t;


#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
