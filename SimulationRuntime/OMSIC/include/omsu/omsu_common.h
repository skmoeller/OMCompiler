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

#include "fmi2/osi_fmi2_wrapper.h"
#include "fmi2/fmi2Functions.h"
#include "omsi.h"

//C Simulation kernel includes
#include "simulation_data.h"
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

#include "openmodelica.h"

#define NUMBER_OF_CATEGORIES 11

#ifdef __cplusplus
extern "C" {
#endif

typedef struct omsi_functions_t{
  /* Function for calling external object deconstructors */
  void (*callExternalObjectDestructors)(DATA *_data, threadData_t*);

  /*! \fn initialNonLinearSystem
   *
   *  This function initialize nonlinear system structure.
   *
   *  \param [ref] [data]
   */
  void (*initialNonLinearSystem)(omsi_int nNonLinearSystems, NONLINEAR_SYSTEM_DATA *data);

  /*! \fn initialLinearSystem
   *
   *  This function initialize nonlinear system structure.
   *
   *  \param [ref] [data]
   */
  void (*initialLinearSystem)(omsi_int nLinearSystems, LINEAR_SYSTEM_DATA *data);

  /*! \fn initialNonLinearSystem
   *
   *  This function initialize nonlinear system structure.
   *
   *  \param [ref] [data]
   */
  void (*initialMixedSystem)(omsi_int nMixedSystems, MIXED_SYSTEM_DATA *data);

  /*! \fn initialNonLinearSystem
   *
   *  This function initialize state set structure.
   *
   *  \param [ref] [data]
   */
  void (*initializeStateSets)(omsi_int nStateSets, STATE_SET_DATA* statesetData, DATA *data);

  /* functionODE contains those equations that are needed
   * to calculate the dynamic part of the system */
  omsi_int (*functionODE)(DATA *data, threadData_t*);

  /* functionAlgebraics contains all continuous equations that
   * are not part of the dynamic part of the system */
  omsi_int (*functionAlgebraics)(DATA *data, threadData_t*);

  /* function for calculating all equation sorting order
     uses in EventHandle  */
  omsi_int (*functionDAE)(DATA *data, threadData_t*);

  /* function that evaluates all localKnownVars  */
  omsi_int (*functionLocalKnownVars)(DATA *data, threadData_t*);

  /* functions for input and output */
  omsi_int (*input_function)(DATA*, threadData_t*);
  omsi_int (*input_function_init)(DATA*, threadData_t*);
  omsi_int (*input_function_updateStartValues)(DATA*, threadData_t*);
  omsi_int (*output_function)(DATA*, threadData_t*);

  /* function for storing value histories of delayed expressions
   * called from functionDAE_output()
   */
  omsi_int (*function_storeDelayed)(DATA *data, threadData_t*);

  /*! \fn updateBoundVariableAttributes
   *
   *  This function updates all bound start, nominal, min, and max values.
   *
   *  \param [ref] [data]
   */
  omsi_int (*updateBoundVariableAttributes)(DATA *data, threadData_t*);

  /*! \fn functionInitialEquations
   *
   * function for calculate initial values from the initial equations and initial algorithms
   *
   *  \param [ref] [data]
   */
  omsi_int (*functionInitialEquations)(DATA *data, threadData_t*);

  /*! \fn functionRemovedInitialEquations
   *
   * This function contains removed equations from the initialization problem,
   * which need to be checked to verify the consistency of the initialization
   * problem.
   *
   *  \param [ref] [data]
   */
  omsi_int (*functionRemovedInitialEquations)(DATA *data, threadData_t*);

  /*! \fn updateBoundParameters
   *
   *  This function calculates bound parameters that depend on other parameters,
   *  e.g. parameter Real n=1/m;
   *  obsolete: bound_parameters
   *
   *  \param [ref] [data]
   */
  omsi_int (*updateBoundParameters)(DATA *data, threadData_t*);

  /* function for checking for asserts and terminate */
  omsi_int (*checkForAsserts)(DATA *data, threadData_t*);

  /*! \fn function_ZeroCrossingsEquations
   *
   *  This function updates all equations to evaluate function_ZeroCrossings
   *
   *  \param [ref] [data]
   */
  omsi_int (*function_ZeroCrossingsEquations)(DATA *data, threadData_t*);

  /*! \fn function_ZeroCrossings
   *
   *  This function evaluates if a sign change occurs at the current state
   *
   *  \param [ref] [data]
   *  \param [ref] [gout]
   */
  omsi_int (*function_ZeroCrossings)(DATA *data, threadData_t *threadData, double* gout);

  /*! \fn function_updateRelations
   *
   *  This function evaluates current continuous relations.
   *
   *  \param [ref] [data]
   *  \param [in]  [evalZeroCross] flag for evaluating Relation with hysteresis
   *                              function or without
   */
  omsi_int (*function_updateRelations)(DATA *data, threadData_t*, omsi_int evalZeroCross);

  /*! \fn checkForDiscreteChanges
   *
   *  This function checks if any discrete variable changed
   *
   *  \param [ref] [data]
   */
  omsi_int (*checkForDiscreteChanges)(DATA *data, threadData_t*);

  /*! \var zeroCrossingDescription
   *
   * This variable contains a description string for zero crossing condition.
   */
  const char *(*zeroCrossingDescription)(omsi_int i, omsi_int **out_EquationIndexes);

  /*! \var relationDescription
   *
   * This variable contains a description string for continuous relations.
   */
  const char *(*relationDescription)(omsi_int i);

  /*! \fn function_initSample
   *
   *  This function initialize the sample-info struct.
   *
   *  \param [ref] [data]
   */
  void (*function_initSample)(DATA *data, threadData_t*);

  /*
   * initialize clocks and subclocks info in modelData
   */
  void (*function_initSynchronous)(DATA * data, threadData_t *threadData);

  /*
   * Update clock interval.
   */
  void (*function_updateSynchronous)(DATA *data, threadData_t *threadData, long i);

  /*
   * Sub-partition's equations
   */
  omsi_int (*function_equationsSynchronous)(DATA *data, threadData_t *threadData, long i);

  /*
   * return input names
   */
  omsi_int (*inputNames)(DATA* modelData, char ** names);

  void (*functionFMIJacobian)(DATA *data, threadData_t*, const unsigned *unknown, omsi_int nUnk, const unsigned *ders, omsi_int nKnown, double *dvKnown, double *out);

} omsi_functions_t;


typedef enum {
  modelInstantiated       = 1<<0, // ME and CS
  modelInitializationMode = 1<<1, // ME and CS
  modelContinuousTimeMode = 1<<2, // ME only
  modelEventMode          = 1<<3, // ME only
  modelSlaveInitialized   = 1<<4, // CS only
  modelTerminated         = 1<<5, // ME and CS
  modelError              = 1<<6  // ME and CS
} ModelState;


// ToDo: is this the right location for these definitions?
typedef void      (*omsi_callback_logger)        	(void*, omsi_string, omsi_status, omsi_string, omsi_string, ...);
typedef void*     (*omsi_callback_allocate_memory)	(omsi_unsigned_int, omsi_unsigned_int);
typedef void      (*omsi_callback_free_memory)    	(void*);
typedef void      (*omsi_step_finished)             (void*, omsi_status);

typedef struct omsi_callback_functions{
	const omsi_callback_logger         	logger;
	const omsi_callback_allocate_memory allocateMemory;
	const omsi_callback_free_memory     freeMemory;
	const omsi_step_finished           	stepFinished;
	const void*							componentEnvironment;
}omsi_callback_functions;


typedef struct osu_t {
    // open modelica simulation interface data
    omsi_t *            osu_data;           /* pointer to omsi_data struct, contains all data for simulation */
    omsi_functions_t *  osu_functions;

    // temporary data for current implementation ToDo: change and delete
    DATA*               old_data;
    threadData_t *      threadData;

    omsi_int            _need_update;
    omsi_int            _has_jacobian;
    ModelState          state;
    omsi_bool           logCategories[NUMBER_OF_CATEGORIES];
    omsi_bool           loggingOn;
    omsi_string         GUID;
    omsi_string         instanceName;
    fmi2EventInfo       eventInfo;
    omsi_bool           toleranceDefined;   // ToDo: delete up to stopTime, redundant information. Already in osu_data->model_info
    omsi_real           tolerance;
    omsi_real           startTime;
    omsi_bool           stopTimeDefined;
    omsi_real           stopTime;
    omsu_type           type;
    omsi_unsigned_int*  vrStates;
    omsi_unsigned_int*  vrStatesDerivatives;

    omsi_callback_functions*  fmiCallbackFunctions;
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
