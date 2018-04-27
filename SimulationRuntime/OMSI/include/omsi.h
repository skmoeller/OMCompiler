#ifndef _OMSI_H
#define _OMSI_H

#include <stdbool.h>

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
	double tollerance;
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
typedef struct {
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
typedef struct {
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
} sim_data_t;

/**
 *
 */
typedef struct {
	unsigned int n_states;
	unsigned int n_derivatives;
	unsigned int n_real_vars;
	unsigned int n_int_vars;
	unsigned int n_bool_vars;
	unsigned int n_string_vars;
	unsigned int n_real_parameters;
	unsigned int n_int_parameters;
	unsigned int n_bool_parameters;
	//number of zero crossings
	unsigned int n_zerocrossings;
	model_variable_info_t* model_vars_info_t;
	equation_info_t*  equation_info_t;
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
