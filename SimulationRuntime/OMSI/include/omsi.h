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


#include "omsi_eqns_system.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * type definitions of variables
 */
#ifdef OSI_FMI2_WRAPPER_H
#include "fmi2/fmi2Functions.h"     // ToDo: delete, is duplicate but silences warnings in my eclipse :-P

typedef fmi2ValueReference  omsi_unsigned_int;
typedef fmi2Real            omsi_real;
typedef fmi2Integer         omsi_int;
typedef fmi2Integer         omsi_long;
typedef fmi2Boolean         omsi_bool;
#define omsi_true  fmi2True
#define omsi_false fmi2False
#ifndef true
#define true fmi2True
#endif
#ifndef false
#define false fmi2False
#endif
typedef fmi2Char            omsi_char;
typedef fmi2String          omsi_string;
typedef fmi2Byte            omsi_char;
#else
typedef unsigned int        omsi_unsigned_int;
typedef double              omsi_real;
typedef int                 omsi_int;
typedef long                omsi_long;
typedef int                 omsi_bool;
#define omsi_true  1
#define omsi_false 0
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
typedef char                omsi_char;
typedef const omsi_char*    omsi_string;
#endif
 /**
  *   for delay handling
  */
struct RINGBUFFER;

// ToDo: is this the right location for these definitions?
typedef enum {
    omsi_model_exchange,
    omsi_co_simulation      // not supported yet
}omsu_type;

typedef enum {
    omsi_ok,
    omsi_warning,
    omsi_discard,
    omsi_error,
    omsi_fatal,
    omsi_pending
}omsi_status;

/**
 *  simulation parameter
 */
typedef struct omsi_experiment_t {
	omsi_real           start_time;     /* start time of experment, default=0 */
	omsi_real           stop_time;      /* end time of experiment, defalut=start+1 */
	omsi_real           step_size;      /* step_size for solvers, default (stop_time-start_time)/500 */
	omsi_unsigned_int   num_outputs;    /* number of outputs of model */
	omsi_real           tolerance;      /* tolerance for solver, default=1e-5 */
	omsi_string         solver_name;    /* name of used solver, default="dassl" */
} omsi_experiment_t;


/**
 *   additional file information for debugging
 */
typedef struct file_info {
	omsi_string filename;       /* filename where variable is defined */
	omsi_int    lineStart;      /* number of line where definition of variable starts */
	omsi_int    colStart;       /* number of columns where definition of variable starts */
	omsi_int    lineEnd;        /* number of line where definition of variable ends */
	omsi_int    colEnd;         /* number of columns where definition of variable ends */
	omsi_bool   fileWritable;   /* =true if file writable, else =false */
} file_info;


/**
 *   additional equation information for debugging
 */
typedef struct equation_info_t{
  omsi_int  id;                 /* unique equation reference from  info.json */
  omsi_int  profileBlockIndex;
  omsi_int  parent;
  omsi_int  numVar;             /* number of unknown variables */
  omsi_int* variables;          /* unknown variables */
} equation_info_t;


/**
 *   variable attributes
 */
typedef enum {
  OMSI_TYPE_UNKNOWN,
  OMSI_TYPE_REAL,
  OMSI_TYPE_INTEGER,
  OMSI_TYPE_BOOLEAN,
  OMSI_TYPE_STRING
}omsi_data_type;


typedef struct omsi_index_type {
  omsi_data_type type;    /* data type*/
  omsi_int       index;   /* index in sim_data->model_vars_and_params->[datatype]
                           * where [datatype]=reals|ints|bools depending on type */
} omsi_index_type;

/**
 * real variable attributes
 */
typedef struct real_var_attribute_t {
	omsi_string unit;          /* default = "" */
	omsi_string displayUnit;   /* default = "" */
	omsi_real   min;           /* default = -Inf */
	omsi_real   max;           /* default = +Inf */
	omsi_bool   fixed;         /* depends on the type */
	omsi_real   nominal;       /* default = 1.0 */
	omsi_real   start;         /* default  = 0.0 */
} real_var_attribute_t;


/**
 * integer variable attributes
 */
typedef struct int_var_attribute_t {
	omsi_int  min;      /* = -Inf */
	omsi_int  max;      /* = +Inf */
	omsi_bool fixed;    /* depends on the type */
	omsi_int  start;    /* = 0 */
} int_var_attribute_t;


/**
 * boolean variable attributes
 */
typedef struct bool_var_attribute_t {
	omsi_bool fixed;    /* depends on the type */
	omsi_bool start;    /* = false */
} bool_var_attribute_t;


/**
 * string variable attributes
 */
typedef struct string_var_attribute_t {
	omsi_char * start;  /* = "" */
} string_var_attribute_t;

/**
 *
 */
typedef struct model_variable_info_t {
	omsi_int        id;						/* unique value reference from *_init.xml */
	omsi_string     name;                   /* name of variable|parameter|alias */
	omsi_string     comment;		        /* variable description  or modelica comment*/
	omsi_index_type type_index;	            /* tuple of data_type and index in sim_data->model_vars_and_params->..., if isAlias=true then index from alias variable */
	void*           modelica_attributes;    /* pointer to modelica attributes  ( real_var_attribute | int_var_attribute | bool_var_attribute | string_var_attribute ) */
	omsi_bool       isAlias;                /* true if alias, else false */
	omsi_int        negate;					/* if negated -1 else 1 */
    omsi_int        aliasID;				/* pointer to alias if >= 0 */
    file_info       info;                   /* pointer to file informations */
} model_variable_info_t;


typedef struct omsi_values {
	omsi_real* reals;       /* array of omsi_real */
	omsi_int*  ints;        /* array of omsi_int */
	omsi_bool* bools;       /* array of omsi_bool */
} omsi_values;


typedef struct equation_system_t {
    omsi_unsigned_int n_output_vars;
    omsi_unsigned_int n_input_vars;
    omsi_unsigned_int n_inner_vars;

    omsi_unsigned_int            n_algebraic_system;     // number of algebraic systems
    omsi_algebraic_system_t* algebraic_system_t;

    omsi_values* equation_vars;

    /* index to sim_data_t->[real|int|bool]_vars */
    omsi_index_type* output_vars_indices;
    omsi_index_type* input_vars_indices;
    omsi_index_type* inner_vars_indices;

    omsi_int (*evaluate) (equation_system_t* equation_system, omsi_values* model_vars_and_params);

} equation_system_t;


/**
 *
 */
typedef struct sim_data_t{

	equation_system_t* initialization;

	equation_system_t* simulation;

	omsi_values* model_vars_and_params;

	//start index of input real variables in real vars array
	omsi_unsigned_int inputs_real_index;
	//start index of input integer variables in real vars array
	omsi_unsigned_int inputs_int_index;
	//start index of input boolean variables in real vars array
	omsi_unsigned_int inputs_bool_index;
	//start index of output real variables in real vars array
	omsi_unsigned_int outputs_real_index;
	//start index of output integer variables in imodel_variable_info_tnteger vars array
	omsi_unsigned_int outputs_int_index;
	//start index of output boolean variables in boolean vars array
	omsi_unsigned_int outputs_bool_index;
	//start index of discrete real variables in real vars array
	omsi_unsigned_int discrete_reals_index;


	omsi_real time_value; // current system time
//	double* pre_real_vars;
//	int* pre_int_vars;
//	bool* pre_bool_vars;
	//conditions of zerocrossing functions
	omsi_bool* zerocrossings_vars;
	//pre conditions of zerocrossing functions
	omsi_bool* pre_zerocrossings_vars;

} sim_data_t;

/**
 *
 */
typedef struct model_data_t {
    omsi_string         modelGUID;
	omsi_unsigned_int   n_states;				// number of continuous states
	omsi_unsigned_int   n_derivatives;          // number of derivatives
	omsi_unsigned_int   n_real_vars;			// number of real algebraic variables
	omsi_unsigned_int   n_real_parameters;		// number of real parameters
	omsi_unsigned_int   n_real_aliases;         // number of real alias variables
	omsi_unsigned_int   n_int_vars;				// number of integer algebraic variables
	omsi_unsigned_int   n_int_parameters;		// number of integer parameters
	omsi_unsigned_int   n_int_aliases;          // number of integer alias variables
	omsi_unsigned_int   n_bool_vars;			// number of boolean algebraic variables
	omsi_unsigned_int   n_bool_parameters;		// number of boolean parameters
	omsi_unsigned_int   n_bool_aliases;         // number of boolean alias variables
	omsi_unsigned_int   n_string_vars;			// number of string algebraic variables
	omsi_unsigned_int	n_string_parameters;	// number of string parameters
	omsi_unsigned_int   n_string_aliases;       // number of string alias variables
	omsi_unsigned_int   n_zerocrossings;        // number of zero crossings
    omsi_unsigned_int   n_equations;            // ToDo: or is this information already somewhere else?

	model_variable_info_t*  model_vars_info_t;		// N = n_states + n_derivatives n_$all_vars + n_$all_parameters  $all={real,int,bool}
	equation_info_t*        equation_info_t;
} model_data_t;

/**
 *
 */
typedef struct omsi_t {
	model_data_t        model_data;
	sim_data_t          sim_data;
	omsi_experiment_t*  experiment;
} omsi_t;

typedef enum {
	omsi_instantiated_mode,
	omsi_initialization_mode,
	omsi_continuousTime_mode,
	omsi_event_mode,
	omsi_none_mode
} omsi_solving_mode_t;


omsi_int omsi_initiatiate_osu(omsi_t** omsi);
omsi_int omsi_initialize_model(omsi_t** omsi);
omsi_int omsi_initialize_solver(omsi_t** omsi);
omsi_int omsi_intialize_simulation(omsi_t** omsi);


#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
