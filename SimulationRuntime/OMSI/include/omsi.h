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


#include "omsi_eqns_system.h"


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

// ToDo: is this the right location for these definitions?
typedef enum {
    omsi_model_exchange,
    omsi_co_simulation      // not supported yet
}omsu_type;

/*
 * Status of actual OpenModelica Simulation Unit
 */
typedef enum {
    omsi_ok,
    omsi_warning,
    omsi_discard,
    omsi_error,
    omsi_fatal,
    omsi_pending
}omsi_status;


typedef struct omsi_index_type {
  omsi_data_type type;    /* data type*/
  omsi_int       index;   /* index in sim_data->model_vars_and_params->[datatype]
                           * where [datatype]=reals|ints|bools depending on type */
} omsi_index_type;


/*
 * Triple of arrays containing actual values for Variables, Parameters, Aliases and so on.
 */
typedef struct omsi_values {
    omsi_real* reals;       /* array of omsi_real */
    omsi_int*  ints;        /* array of omsi_int */
    omsi_bool* bools;       /* array of omsi_bool */
} omsi_values;



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
    omsi_int        id;                 /* unique equation reference from  info.json */
    omsi_int        profileBlockIndex;
    omsi_int        parent;
    omsi_int        numVar;             /* number of defining variables */
    omsi_string*    variables;          /* array of unknown variables */
    file_info       info;               /* file informations for  equation */
} equation_info_t;


/**
 *  Modelica attributes for real variables
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
 *  Modelica attributes for integer variables
 */
typedef struct int_var_attribute_t {
	omsi_int  min;      /* = -Inf */
	omsi_int  max;      /* = +Inf */
	omsi_bool fixed;    /* depends on the type */
	omsi_int  start;    /* = 0 */
} int_var_attribute_t;


/**
 *  Modelica attributes for boolean variables
 */
typedef struct bool_var_attribute_t {
	omsi_bool fixed;    /* depends on the type */
	omsi_bool start;    /* = false */
} bool_var_attribute_t;


/**
 * Modelica attributes for string variables
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
    file_info       info;                   /* file informations for variable|parameter|alias */
} model_variable_info_t;



typedef struct omsi_function_t {
    omsi_unsigned_int n_output_vars;
    omsi_unsigned_int n_input_vars;
    omsi_unsigned_int n_inner_vars;

    omsi_unsigned_int           n_algebraic_system;     // number of algebraic systems
    omsi_algebraic_system_t*    algebraic_system_t;

    omsi_values* function_vars;

    /* index to sim_data_t->[real|int|bool]_vars */
    omsi_index_type* output_vars_indices;
    omsi_index_type* input_vars_indices;
    omsi_index_type* inner_vars_indices;

    omsi_int (*evaluate) (omsi_function_t* this_function, omsi_values* model_vars_and_params);
} omsi_function_t;


/**
 * Structure containing all static simulation informations.
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
    omsi_unsigned_int   n_equations;            // number of all equations
    omsi_unsigned_int	n_regular_equations;	// number of regular equations
    omsi_unsigned_int	n_init_equations;		// number of initial equations ToDo: fill with value somewhere

	model_variable_info_t*  model_vars_info_t;		// N = n_states + n_derivatives n_$all_vars + n_$all_parameters  $all={real,int,bool}
	equation_info_t*        equation_info_t;
} model_data_t;


/**
 * Structure containing all dynamic simulation data.
 */
typedef struct sim_data_t{
    omsi_function_t* initialization;        // array of all functions structures necessary for initialization
    omsi_function_t* simulation;            // array of all functions structures necessary for simulation

    omsi_values* model_vars_and_params;     // pointer to struct of arrays containing values for all variables, parameters and so on

    omsi_unsigned_int inputs_real_index;    //start index of input real variables in real vars array
    omsi_unsigned_int inputs_int_index;     //start index of input integer variables in real vars array
    omsi_unsigned_int inputs_bool_index;    //start index of input boolean variables in real vars array
    omsi_unsigned_int outputs_real_index;   //start index of output real variables in real vars array
    omsi_unsigned_int outputs_int_index;    //start index of output integer variables in imodel_variable_info_tnteger vars array
    omsi_unsigned_int outputs_bool_index;   //start index of output boolean variables in boolean vars array
    omsi_unsigned_int discrete_reals_index; //start index of discrete real variables in real vars array

    omsi_real time_value;                   // current system time

    omsi_bool* zerocrossings_vars;          //conditions of zerocrossing functions
    omsi_bool* pre_zerocrossings_vars;      //pre conditions of zerocrossing functions
} sim_data_t;


/**
 *  Experiment parameters
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
 * Central structure containing all informations
 */
typedef struct omsi_t {
	model_data_t        model_data;
	sim_data_t          sim_data;
	omsi_experiment_t*  experiment;
} omsi_t;

omsi_int omsi_initiatiate_osu(omsi_t** omsi);
omsi_int omsi_initialize_model(omsi_t** omsi);
omsi_int omsi_initialize_solver(omsi_t** omsi);
omsi_int omsi_intialize_simulation(omsi_t** omsi);


#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
