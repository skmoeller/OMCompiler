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

#include "../OMSI/include/omsi.h"
#include "omsu_common.h"
#include "fmi2/osi_fmi2_wrapper.h"      // ToDo: Should not be needed here
#include "fmi2/fmi2Functions.h"

#include <expat.h>
#include "util/uthash.h"

/* forward struct */    // ToDo: Sort header files in a better way. Ther are always some loops...
typedef struct osu_t osu_t;
typedef struct omsi_t omsi_t;


// ToDo: include all headers neccesary for XML parser, delete others

//C Simulation kernel includes
#include "../simulation_data.h"
//#include "../simulation/solver/stateset.h"
//#include "../simulation/solver/model_help.h"
//#if !defined(OMC_NUM_NONLINEAR_SYSTEMS) || OMC_NUM_NONLINEAR_SYSTEMS>0
//#include "../simulation/solver/nonlinearSystem.h"
//#endif
//#if !defined(OMC_NUM_LINEAR_SYSTEMS) || OMC_NUM_LINEAR_SYSTEMS>0
//#include "../simulation/solver/linearSystem.h"
//#endif
//#if !defined(OMC_NUM_MIXED_SYSTEMS) || OMC_NUM_MIXED_SYSTEMS>0
//#include "../simulation/solver/mixedSystem.h"
//#endif
//#include "../simulation/solver/delay.h"
//#include "../simulation/solver/initialization/initialization.h"
//#include "../simulation/simulation_info_json.h"
//#include "../simulation/simulation_input_xml.h"



/* some stuff from simulation_input_xml.c
 * ToDo: implement for new data struct
 */
typedef struct hash_string_string
{
  omsi_string id;
  omsi_string val;
  UT_hash_handle hh;
} hash_string_string;

typedef hash_string_string omc_ModelDescription;
typedef hash_string_string omc_DefaultExperiment;
typedef hash_string_string omc_ScalarVariable;

typedef struct hash_long_var
{
  omsi_long id;
  omc_ScalarVariable *val;
  UT_hash_handle hh;
} hash_long_var;

typedef hash_long_var omc_ModelVariables;

typedef struct hash_string_long
{
  omsi_string id;
  omsi_long val;
  UT_hash_handle hh;
} hash_string_long;

/* structure used to collect data from the xml input file */
typedef struct omc_ModelInput
{
  omc_ModelDescription  *md; /* model description */
  omc_DefaultExperiment *de; /* default experiment */

  omc_ModelVariables    *rSta; /* states */
  omc_ModelVariables    *rDer; /* derivatives */
  omc_ModelVariables    *rAlg; /* algebraic */
  omc_ModelVariables    *rPar; /* parameters */
  omc_ModelVariables    *rAli; /* aliases */
  omc_ModelVariables    *rSen; /* sensitivities */

  omc_ModelVariables    *iAlg; /* int algebraic */
  omc_ModelVariables    *iPar; /* int parameters */
  omc_ModelVariables    *iAli; /* int aliases */

  omc_ModelVariables    *bAlg; /* bool algebraic */
  omc_ModelVariables    *bPar; /* bool parameters */
  omc_ModelVariables    *bAli; /* bool aliases */

  omc_ModelVariables    *sAlg; /* string algebraic */
  omc_ModelVariables    *sPar; /* string parameters */
  omc_ModelVariables    *sAli; /* string aliases */

  /* these two we need to know to be able to add
     the stuff in <Real ... />, <String ... /> to
     the correct variable in the correct map */
  mmc_sint_t            lastCI; /* index */
  omc_ModelVariables**  lastCT; /* type (classification) */
} omc_ModelInput;



#include "openmodelica.h"

#ifdef __cplusplus
extern "C" {
#endif

void omsu_print_debug (osu_t* OSU);     // ToDo: delete later
omsi_int omsu_allocate_sim_data(omsi_t* omsi_data, const fmi2CallbackAllocateMemory allocateMemory);
void omsu_free_osu_data(omsi_t* omsi_data,const fmi2CallbackFreeMemory freeMemory);
void storePreValues (DATA *data);
omsi_int stateSelection(DATA *data, threadData_t *threadData, omsi_char reportError, omsi_int switchStates);
void overwriteOldSimulationData(DATA *data);
void initializeDataStruc(DATA *data, threadData_t *threadData);
omsi_int omsu_process_input_xml(omsi_t* osu_data, omsi_char* filename, fmi2String fmuGUID, fmi2String instanceName, const fmi2CallbackFunctions* functions);
omsi_int initializeNonlinearSystems(DATA *data, threadData_t *threadData);
omsi_int initializeLinearSystems(DATA *data, threadData_t *threadData);
omsi_int initializeMixedSystems(DATA *data, threadData_t *threadData);
void initializeStateSetJacobians(DATA *data, threadData_t *threadData);
void setZCtol(omsi_real relativeTol);
void copyStartValuestoInitValues(DATA *data);
omsi_int initialization(DATA *data, threadData_t *threadData, omsi_string pInitMethod, omsi_string pInitFile, omsi_real initTime);
void initSample(DATA* data, threadData_t *threadData, omsi_real startTime, omsi_real stopTime);
void initDelay(DATA* data, omsi_real startTime);
omsi_real getNextSampleTimeFMU(DATA *data);
void setAllVarsToStart(DATA *data);
void setAllParamsToStart(DATA *data);
omsi_int freeNonlinearSystems(DATA *data, threadData_t *threadData);
omsi_int freeMixedSystems(DATA *data, threadData_t *threadData);
omsi_int freeLinearSystems(DATA *data, threadData_t *threadData);
void freeStateSetData(DATA *data);
void deInitializeDataStruc(DATA *data);
void updateRelationsPre(DATA *data);
modelica_boolean checkRelations(DATA *data);

void mmc_catch_dummy_fn (void);

extern void omsic_model_setup_data (osu_t* OSU);

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
