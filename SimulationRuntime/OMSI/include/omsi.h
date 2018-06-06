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
typedef struct omsi_experiment_t {
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
typedef struct file_info {
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
typedef struct equation_info_t{
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
typedef struct real_var_attribute_t {
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
typedef struct int_var_attribute_t {
	int min; /* = -Inf */
	int max; /* = +Inf */
	bool fixed; /* depends on the type */
	bool useStart; /* = false */
	int start; /* = 0 */
} int_var_attribute_t;


/**
 * boolean variable attributes
 */
typedef struct bool_var_attribute_t {
	bool fixed; /* depends on the type */
	bool useStart; /* = false */
	bool start; /* = false */
} bool_var_attribute_t;


/**
 * string variable attributes
 */
typedef struct string_var_attribute_t {
	bool useStart; /* = false */
	bool start; /* = "" */
} string_var_attribute_t;


/**
 *
 */
typedef struct model_variable_info_t {
	int id;
	const char *name;
	const char *comment;
	int var_type;
	void* attribute;    //pointer to variable attribute  ( real_var_attribute | int_var_attribute | bool_var_attribute | string_var_attribute )
	file_info info;
} model_variable_info_t;


typedef enum {
  OMSI_TYPE_UNKNOWN,
  OMSI_TYPE_DOUBLE,
  OMSI_TYPE_INTEGER,
  OMSI_TYPE_BOOLEAN,
  OMSI_TYPE_STRING
}omsi_data_type;


typedef struct omsi_index_type {
  omsi_data_type type;
  int index;
} omsi_index_type;


typedef struct omsi_values {
  double* reals;
  int* ints;
  bool* bools;
} omsi_values;


typedef struct equation_system_t {

    unsigned int n_output_vars;
    unsigned int n_input_vars;
    unsigned int n_inner_vars;

    unsigned int            n_algebraic_system;     // number of algebraic systems
    omsi_algebraic_system_t* algebraic_system_t;

    omsi_values* equation_vars;

    /* index to sim_data_t->[real|int|bool]_vars */
    omsi_index_type* output_vars_indices;
    omsi_index_type* input_vars_indices;
    omsi_index_type* inner_vars_indices;

    int (*evaluate) (equation_system_t* equation_system, omsi_values* model_vars_and_params);

} equation_system_t;


/**
 *
 */
typedef struct sim_data_t{

	equation_system_t* initialization;

	equation_system_t* simulation;

	omsi_values* model_vars_and_params;

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
//	double* pre_real_vars;
//	int* pre_int_vars;
//	bool* pre_bool_vars;
	//conditions of zerocrossing functions
	bool* zerocrossings_vars;
	//pre conditions of zerocrossing functions
	bool* pre_zerocrossings_vars;

} sim_data_t;

/**
 *
 */
typedef struct model_data_t {
    const char*             modelGUID;
	unsigned int            n_states;				// number of continuous states
	unsigned int            n_derivatives;
	unsigned int            n_real_vars;			// number of real algebraic variables
	unsigned int            n_int_vars;				// number of integer algebraic variables
	unsigned int            n_bool_vars;			// number of boolean algebraic variables
	unsigned int            n_string_vars;			// number of string algebraic variables
	unsigned int            n_real_parameters;		// number of real parameters
	unsigned int            n_int_parameters;		// number of integer parameters
	unsigned int            n_bool_parameters;		// number of boolean parameters
	unsigned int			n_string_parameters;	// number of string parameters
	unsigned int            n_zerocrossings;        // number of zero crossings
    unsigned int            n_equations;            // ToDo: or is this information already somewhere else?

	model_variable_info_t*  model_vars_info_t;		// N = n_$all_vars + n_$all_parameters  $all={real,int,bool}
	equation_info_t*        equation_info_t;
} model_data_t;

/**
 *
 */
typedef struct omsi_t {
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
