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
 * This header file contains structs and forwards extern functions for
 * all other OMSU c files.
 */
#ifndef OMSU_COMMON_H
#define OMSU_COMMON_H

#include "omsi.h"


#define NUMBER_OF_CATEGORIES 11

#ifdef __cplusplus
extern "C" {
#endif

typedef struct omsi_functions_t omsi_functions_t; /* ToDo: add definition here */

typedef enum {
  modelInstantiated       = 1<<0, /* ME and CS */
  modelInitializationMode = 1<<1, /* ME and CS */
  modelContinuousTimeMode = 1<<2, /* ME only */
  modelEventMode          = 1<<3, /* ME only */
  modelSlaveInitialized   = 1<<4, /* CS only */
  modelTerminated         = 1<<5, /* ME and CS */
  modelError              = 1<<6  /* ME and CS */
} ModelState;


typedef struct {
   omsi_bool newDiscreteStatesNeeded;
   omsi_bool terminateSimulation;
   omsi_bool nominalsOfContinuousStatesChanged;
   omsi_bool valuesOfContinuousStatesChanged;
   omsi_bool nextEventTimeDefined;
   omsi_real nextEventTime;
} omsi_event_info;

typedef struct osu_t {
    /* open modelica simulation interface data */
    omsi_t*             osu_data;           /* pointer to omsi_data struct, contains all data for simulation */
    omsi_functions_t*   osu_functions;

    omsi_int            _need_update;
    omsi_int            _has_jacobian;
    ModelState          state;
    omsi_bool           logCategories[NUMBER_OF_CATEGORIES];
    omsi_bool           loggingOn;
    omsi_char*          GUID;
    omsi_string         instanceName;
    omsi_event_info     eventInfo;
    omsi_bool           toleranceDefined;   /* ToDo: delete up to stopTime, redundant information. Already in osu_data->model_info */
    omsi_real           tolerance;
    omsi_real           startTime;
    omsi_bool           stopTimeDefined;
    omsi_real           stopTime;
    omsu_type           type;
    omsi_unsigned_int*  vrStates;
    omsi_unsigned_int*  vrStatesDerivatives;

    const omsi_callback_functions*  fmiCallbackFunctions;
} osu_t;

/* extern functions */
extern void omsic_model_setup_data(osu_t* OSU);
extern omsi_status setExternalFunction(osu_t* OSU, const omsi_unsigned_int vr, const void* value);
extern void mmc_catch_dummy_fn(void);


/* function prototypes */
omsi_real getReal(osu_t* OSU, const omsi_unsigned_int vr);
omsi_status setReal(osu_t* OSU, const omsi_unsigned_int vr, const omsi_real value);
omsi_int getInteger(osu_t* OSU, const omsi_unsigned_int vr);
omsi_status setInteger(osu_t* OSU, const omsi_unsigned_int vr, const omsi_int value);
omsi_bool getBoolean(osu_t* OSU, const omsi_unsigned_int vr);
omsi_status setBoolean(osu_t* OSU, const omsi_unsigned_int vr, const omsi_bool value);
omsi_string getString(osu_t* OSU, const omsi_unsigned_int vr);
omsi_status setString(osu_t* OSU, const omsi_unsigned_int vr, omsi_string value);

#ifdef __cplusplus
extern } /* end of extern "C" */
#endif

#endif
