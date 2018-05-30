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

#ifndef OMSU_HELPER_H
#define OMSU_HELPER_H

#include "omsu_common.h"

/* forward struct */
typedef struct osu_t osu_t;

// ToDo: include all headers neccesary for XML parser, delete others

//C Simulation kernel includes
#include "../simulation_data.h"
#include "../simulation/solver/stateset.h"
#include "../simulation/solver/model_help.h"
#if !defined(OMC_NUM_NONLINEAR_SYSTEMS) || OMC_NUM_NONLINEAR_SYSTEMS>0
#include "../simulation/solver/nonlinearSystem.h"
#endif
#if !defined(OMC_NUM_LINEAR_SYSTEMS) || OMC_NUM_LINEAR_SYSTEMS>0
#include "../simulation/solver/linearSystem.h"
#endif
#if !defined(OMC_NUM_MIXED_SYSTEMS) || OMC_NUM_MIXED_SYSTEMS>0
#include "../simulation/solver/mixedSystem.h"
#endif
#include "../simulation/solver/delay.h"
#include "../simulation/solver/initialization/initialization.h"
#include "../simulation/simulation_info_json.h"
#include "../simulation/simulation_input_xml.h"

#include "openmodelica.h"


void storePreValues (DATA *data);
int stateSelection(DATA *data, threadData_t *threadData, char reportError, int switchStates);
void overwriteOldSimulationData(DATA *data);
void initializeDataStruc(DATA *data, threadData_t *threadData);
void read_input_xml(MODEL_DATA* modelData, SIMULATION_INFO* simulationInfo);
int initializeNonlinearSystems(DATA *data, threadData_t *threadData);
int initializeLinearSystems(DATA *data, threadData_t *threadData);
int initializeMixedSystems(DATA *data, threadData_t *threadData);
void initializeStateSetJacobians(DATA *data, threadData_t *threadData);
void setZCtol(double relativeTol);
void copyStartValuestoInitValues(DATA *data);
int initialization(DATA *data, threadData_t *threadData, const char* pInitMethod, const char* pInitFile, double initTime);
void initSample(DATA* data, threadData_t *threadData, double startTime, double stopTime);
void initDelay(DATA* data, double startTime);
double getNextSampleTimeFMU(DATA *data);
void setAllVarsToStart(DATA *data);
void setAllParamsToStart(DATA *data);
int freeNonlinearSystems(DATA *data, threadData_t *threadData);
int freeMixedSystems(DATA *data, threadData_t *threadData);
int freeLinearSystems(DATA *data, threadData_t *threadData);
void freeStateSetData(DATA *data);
void deInitializeDataStruc(DATA *data);
void updateRelationsPre(DATA *data);
modelica_boolean checkRelations(DATA *data);

void mmc_catch_dummy_fn (void);
void omsic_model_setup_data (osu_t* OSU);

#endif
