#ifndef _OMSI_EQNS_SYSTEM_H
#define _OMSI_EQNS_SYSTEM_H

#include "omsi.h"
#include "../../OMSIC/include/math/omsi_math.h"
#include "../../OMSIC/include/math/omsi_matrix.h"
#include "../../OMSIC/include/math/omsi_vector.h"
//#include "omsi_jacobian.h"


#ifdef __cplusplus
extern "C" {
#endif


/* forward some types from omsi.h */
typedef struct sim_data_t sim_data_t;
typedef struct equation_info_t equation_info_t;
typedef struct omsi_function_t omsi_function_t;
typedef struct omsi_index_type omsi_index_type;

/* forward some types */
typedef struct omsi_linear_system_t omsi_linear_system_t;
typedef struct omsi_sparsity_pattern omsi_sparsity_pattern;
typedef struct omsi_analytical_jacobian omsi_analytical_jacobian;

/* function prototypes for omsi_linear_system_t functions */
typedef omsi_int (*omsi_linear_system_t_get_x)(sim_data_t* data, omsi_vector_t* vector);


/**
 *
 */
typedef struct omsi_analytical_jacobian{
    omsi_unsigned_int n_columns;
    omsi_unsigned_int n_rows;
    omsi_real* tmp_vars;
    omsi_real* seed_vars;
    omsi_real* result_vars;

    omsi_function_t* functions;
    omsi_int (*directionalDerivative) (omsi_function_t *real_vars, omsi_analytical_jacobian *jacobian);
    //sparsity_pattern_t* sparsity_pattern;
}omsi_analytical_jacobian;

/**
 * general algebraic system
 */
typedef struct {
    equation_info_t* info;
    omsi_unsigned_int n_iteration_vars;
    omsi_unsigned_int n_input_vars;
    omsi_unsigned_int n_inner_vars;

    omsi_unsigned_int n_conditions;
    omsi_int *zerocrossing_indices;

    omsi_bool isLinear;      // linear system=true and non-linear system=false
    void *loop;        // points on array of omsi_linear_loop or omsi_non-linear_loop

    omsi_function_t* functions;

}omsi_algebraic_system_t;


/**
 * linear system
 */
typedef struct {
    //model_variables_info_t *info;
    void *solverData;
    omsi_analytical_jacobian *jacobian;

}omsi_linear_loop;

/**
 * non-linear system
 */
typedef struct {
    // ToDo: complete
}omsi_nonlinear_loop;


#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
