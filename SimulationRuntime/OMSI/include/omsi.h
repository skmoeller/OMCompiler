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

#ifndef _OMSI_H
#define _OMSI_H

#include <stdbool.h>
#include "omsi_eqns_system.h"

#ifdef __cplusplus
extern "C" {
#endif


 /**
  *   for delay handling
  */
struct RINGBUFFER;

/**
 *  simulation parameter
 */
typedef struct {
	double start_time;
	double stop_time;
	double step_size;
	unsigned int num_outputs;
	double tolerance;
	const char * solver_name;
} omsi_experiment_t;

/**
 *   additional file information for debugging
 */
typedef struct {
	const char* filename;
	int lineStart;
	int colStart;
	int lineEnd;
	int colEnd;
	int readonly;
} file_info;

/**
 *   additional equation information for debugging
 */
typedef struct _equation_info_t{
  int id;
  int profileBlockIndex;
  int parent;
  int numVar;      // number of unknown variables
  int* variables;  // unknown variables
} equation_info_t;

/**
 *   variable attributes
 */
typedef enum {
  TYPE_UNKNOWN = 0,

  TYPE_REAL,
  TYPE_INTEGER,
  TYPE_BOOL,
  TYPE_STRING,

  TYPE_MAX
} var_type;

/**
 * real variable attributes
 */
typedef struct {
	const char * unit; /* = "" */
	const char * displayUnit; /* = "" */
	double min; /* = -Inf */
	double max; /* = +Inf */
	bool fixed; /* depends on the type */
	bool useNominal; /* = false */
	double nominal; /* = 1.0 */
	bool useStart; /* = false */
	double start; /* = 0.0 */
} real_var_attribute_t;

/**
 * integer variable attributes
 */
typedef struct {
	int min; /* = -Inf */
	int max; /* = +Inf */
	bool fixed; /* depends on the type */
	bool useStart; /* = false */
	int start; /* = 0 */
} int_var_attribute_t;

/**
 * boolean variable attributes
 */
typedef struct {
	bool fixed; /* depends on the type */
	bool useStart; /* = false */
	bool start; /* = false */
} bool_var_attribute_t;

/**
 * string variable attributes
 */
typedef struct {
	bool useStart; /* = false */
	bool start; /* = "" */
} string_var_attribute_t;

/**
 *
 */
typedef struct {
	int id;
	const char *name;
	const char *comment;
	int var_type;
	//pointer to variable attribute e.g real_var_attribute
	void* attribute;
	file_info info;
} model_variable_info_t;

/**
 *
 */
typedef struct _sim_data_t{
	double* real_vars;
	int* int_vars;
	bool* bool_vars;
	//start index of state variables in real vars array
	unsigned int states_index;
	//start index of derivative variables in real vars array
	unsigned int der_states_index;
	//start index of input real variables in real vars array
	unsigned int inputs_real_index;
	//start index of input integer variables in real vars array
	unsigned int inputs_int_index;
	//start index of input boolean variables in real vars array
	unsigned int inputs_bool_index;
	//start index of output real variables in real vars array
	unsigned int outputs_real_index;
	//start index of output integer variables in integer vars array
	unsigned int outputs_int_index;
	//start index of output boolean variables in boolean vars array
	unsigned int outputs_bool_index;
	//start index of discrete real variables in real vars array
	unsigned int discrete_reals_index;


	double time_value; // current system time
	double* pre_real_vars;
	int* pre_int_vars;
	bool* pre_bool_vars;
	//conditions of zerocrossing functions
	bool* zerocrossings_vars;
	//pre conditions of zerocrossing functions
	bool* pre_zerocrossings_vars;

	int state;  // current state in fmi2 functions
} sim_data_t;

/**
 *
 */
typedef struct {
    char*                   modelGUID;
	unsigned int            n_states;
	unsigned int            n_derivatives;
	unsigned int            n_real_vars;
	unsigned int            n_int_vars;
	unsigned int            n_bool_vars;
	unsigned int            n_string_vars;
	unsigned int            n_real_parameters;
	unsigned int            n_int_parameters;
	unsigned int            n_bool_parameters;
	unsigned int            n_zerocrossings;        //number of zero crossings
	model_variable_info_t*  model_vars_info_t;
	equation_info_t*        equation_info_t;

	omsi_algebraic_system_t* algebraic_system_t;
	unsigned int            n_algebraic_system;
} model_data_t;

/**
 *
 */
typedef struct {
	model_data_t model_data;
	sim_data_t sim_data;
	omsi_experiment_t* experiment;
} omsi_t;

int omsi_initiatiate_osu(omsi_t** omsi);
int omsi_initialize_model(omsi_t** omsi);
int omsi_initialize_solver(omsi_t** omsi);
int omsi_intialize_simulation(omsi_t** omsi);


#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
