#ifndef OSI_OSU_H
#define OSI_OSU_H

#include "osu_utils.h"
#include "osu_GettersAndSetters.h"
#include "osu_EventSimulation.h"
#include "osu_ContinuousSimulation.h"
#include "osu_Initialization.h"

#include "fmi2/fmi2Functions.h"
#include "omsi.h"

#include "simulation_data.h"
#include "openmodelica.h"

#define NUMBER_OF_CATEGORIES 11
#ifdef __cplusplus
extern "C" {
#endif

typedef struct omsi_functions_t {

  /* Function for calling external object deconstructors */
  void (*callExternalObjectDestructors)(DATA *_data, threadData_t*);

  /*! \fn initialNonLinearSystem
   *
   *  This function initialize nonlinear system structure.
   *
   *  \param [ref] [data]
   */
  void (*initialNonLinearSystem)(int nNonLinearSystems, NONLINEAR_SYSTEM_DATA *data);

  /*! \fn initialLinearSystem
   *
   *  This function initialize nonlinear system structure.
   *
   *  \param [ref] [data]
   */
  void (*initialLinearSystem)(int nLinearSystems, LINEAR_SYSTEM_DATA *data);

  /*! \fn initialNonLinearSystem
   *
   *  This function initialize nonlinear system structure.
   *
   *  \param [ref] [data]
   */
  void (*initialMixedSystem)(int nMixedSystems, MIXED_SYSTEM_DATA *data);

  /*! \fn initialNonLinearSystem
   *
   *  This function initialize state set structure.
   *
   *  \param [ref] [data]
   */
  void (*initializeStateSets)(int nStateSets, STATE_SET_DATA* statesetData, DATA *data);

  /* functionODE contains those equations that are needed
   * to calculate the dynamic part of the system */
  int (*functionODE)(DATA *data, threadData_t*);

  /* functionAlgebraics contains all continuous equations that
   * are not part of the dynamic part of the system */
  int (*functionAlgebraics)(DATA *data, threadData_t*);

  /* function for calculating all equation sorting order
     uses in EventHandle  */
  int (*functionDAE)(DATA *data, threadData_t*);

  /* function that evaluates all localKnownVars  */
  int (*functionLocalKnownVars)(DATA *data, threadData_t*);

  /* functions for input and output */
  int (*input_function)(DATA*, threadData_t*);
  int (*input_function_init)(DATA*, threadData_t*);
  int (*input_function_updateStartValues)(DATA*, threadData_t*);
  int (*output_function)(DATA*, threadData_t*);

  /* function for storing value histories of delayed expressions
   * called from functionDAE_output()
   */
  int (*function_storeDelayed)(DATA *data, threadData_t*);

  /*! \fn updateBoundVariableAttributes
   *
   *  This function updates all bound start, nominal, min, and max values.
   *
   *  \param [ref] [data]
   */
  int (*updateBoundVariableAttributes)(DATA *data, threadData_t*);

  /*! \fn functionInitialEquations
   *
   * function for calculate initial values from the initial equations and initial algorithms
   *
   *  \param [ref] [data]
   */
  int (*functionInitialEquations)(DATA *data, threadData_t*);

  /*! \fn functionRemovedInitialEquations
   *
   * This function contains removed equations from the initialization problem,
   * which need to be checked to verify the consistency of the initialization
   * problem.
   *
   *  \param [ref] [data]
   */
  int (*functionRemovedInitialEquations)(DATA *data, threadData_t*);

  /*! \fn updateBoundParameters
   *
   *  This function calculates bound parameters that depend on other parameters,
   *  e.g. parameter Real n=1/m;
   *  obsolete: bound_parameters
   *
   *  \param [ref] [data]
   */
  int (*updateBoundParameters)(DATA *data, threadData_t*);

  /* function for checking for asserts and terminate */
  int (*checkForAsserts)(DATA *data, threadData_t*);

  /*! \fn function_ZeroCrossingsEquations
   *
   *  This function updates all equations to evaluate function_ZeroCrossings
   *
   *  \param [ref] [data]
   */
  int (*function_ZeroCrossingsEquations)(DATA *data, threadData_t*);

  /*! \fn function_ZeroCrossings
   *
   *  This function evaluates if a sign change occurs at the current state
   *
   *  \param [ref] [data]
   *  \param [ref] [gout]
   */
  int (*function_ZeroCrossings)(DATA *data, threadData_t *threadData, double* gout);

  /*! \fn function_updateRelations
   *
   *  This function evaluates current continuous relations.
   *
   *  \param [ref] [data]
   *  \param [in]  [evalZeroCross] flag for evaluating Relation with hysteresis
   *                              function or without
   */
  int (*function_updateRelations)(DATA *data, threadData_t*, int evalZeroCross);

  /*! \fn checkForDiscreteChanges
   *
   *  This function checks if any discrete variable changed
   *
   *  \param [ref] [data]
   */
  int (*checkForDiscreteChanges)(DATA *data, threadData_t*);

  /*! \var zeroCrossingDescription
   *
   * This variable contains a description string for zero crossing condition.
   */
  const char *(*zeroCrossingDescription)(int i, int **out_EquationIndexes);

  /*! \var relationDescription
   *
   * This variable contains a description string for continuous relations.
   */
  const char *(*relationDescription)(int i);

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
  int (*function_equationsSynchronous)(DATA *data, threadData_t *threadData, long i);

  /*
   * return input names
   */
  int (*inputNames)(DATA* modelData, char ** names);

  void (*functionFMIJacobian)(DATA *data, threadData_t*, const unsigned *unknown, int nUnk, const unsigned *ders, int nKnown, double *dvKnown, double *out);

} omsi_functions_t;

//TODO: adapt the states (and probably functions) to fmi2!
typedef enum {
  modelInstantiated       = 1<<0, // ME and CS
  modelInitializationMode = 1<<1, // ME and CS
  modelContinuousTimeMode = 1<<2, // ME only
  modelEventMode          = 1<<3, // ME only
  modelSlaveInitialized   = 1<<4, // CS only
  modelTerminated         = 1<<5, // ME and CS
  modelError              = 1<<6  // ME and CS
} ModelState;

/*
typedef enum {
    modelStartAndEnd        = 1<<0,
    modelInstantiated       = 1<<1,
    modelInitializationMode = 1<<2,

    // ME states
    modelEventMode          = 1<<3,
    modelContinuousTimeMode = 1<<4,
    // CS states
    modelStepComplete       = 1<<5,
    modelStepInProgress     = 1<<6,
    modelStepFailed         = 1<<7,
    modelStepCanceled       = 1<<8,

    modelTerminated         = 1<<9,
    modelError              = 1<<10,
    modelFatal              = 1<<11,
} ModelState;
*/

typedef struct osu_t
{
  // open modelica simulation unit data
	omsi_t                        osu_data;

	omsi_functions_t*       osu_functions;

	// temporary data for current implementation
	DATA* old_data;
	threadData_t *threadData;

	int _need_update;
	int _has_jacobian;
	ModelState state;
	fmi2Boolean logCategories[NUMBER_OF_CATEGORIES];
	fmi2Boolean loggingOn;
	fmi2String GUID;
	fmi2String instanceName;
	fmi2EventInfo eventInfo;
	fmi2Boolean toleranceDefined;
	fmi2Real tolerance;
	fmi2Real startTime;
	fmi2Boolean stopTimeDefined;
	fmi2Real stopTime;
	fmi2Type type;
	fmi2ValueReference* vrStates;
	fmi2ValueReference* vrStatesDerivatives;

	fmi2CallbackFunctions* fmiCallbackFunctions;
} osu_t;

fmi2Status fmi2EventUpdate(fmi2Component c, fmi2EventInfo* eventInfo);

extern void omsic_model_setup_data(osu_t* OSU);
extern fmi2Real getReal(osu_t* OSU, const fmi2ValueReference vr);
extern fmi2Status setReal(osu_t* OSU, const fmi2ValueReference vr, const fmi2Real value);
extern fmi2Integer getInteger(osu_t* OSU, const fmi2ValueReference vr);
extern fmi2Status setInteger(osu_t* OSU, const fmi2ValueReference vr, const fmi2Integer value);
extern fmi2Boolean getBoolean(osu_t* OSU, const fmi2ValueReference vr);
extern fmi2Status setBoolean(osu_t* OSU, const fmi2ValueReference vr, const fmi2Boolean value);
extern fmi2String getString(osu_t* OSU, const fmi2ValueReference vr);
extern fmi2Status setString(osu_t* OSU, const fmi2ValueReference vr, fmi2String value);
extern fmi2Status setExternalFunction(osu_t* OSU, const fmi2ValueReference vr, const void* value);

//unsupported functions

/*! \fn omsi_free_fmu_state
 *
 *  This function is not supported.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_free_fmu_state(fmi2Component c, fmi2FMUstate* FMUstate);

/*! \fn omsi_serialized_fmu_state_size
 *
 *  This function is not supported.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_serialized_fmu_state_size(fmi2Component c, fmi2FMUstate FMUstate,
                                          size_t* size);

/*! \fn omsi_serialize_fmu_state
 *
 *  This function is not supported.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_serialize_fmu_state(fmi2Component c, fmi2FMUstate FMUstate,
                                    fmi2Byte serializedState[], size_t size);

/*! \fn omsi_de_serialize_fmu_state
 *
 *  This function is not supported.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_de_serialize_fmu_state(fmi2Component c,
                                       const fmi2Byte serializedState[],
                                       size_t size, fmi2FMUstate* FMUstate);


#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
