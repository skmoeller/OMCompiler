#ifndef _OMSI_EQNS_SYSTEM_H
#define _OMSI_EQNS_SYSTEM_H

#include <stdbool.h>
#include "../../c/omsi/Solver/omsi_math/omsi_math.h"
#include "../../c/omsi/Solver/omsi_math/omsi_vector.h"
#include "../../c/omsi/Solver/omsi_math/omsi_matrix.h"
#include "omsi_jacobian.h"
//#include "omsi.h"   //ToDo: should be pretty wrong

#ifdef __cplusplus
extern "C" {
#endif


/* forward some types */
typedef struct sim_data_t sim_data_t;
typedef struct _equation_info_t equation_info_t;
typedef struct _omsi_linear_system_t omsi_linear_system_t;
typedef struct omsi_sparsity_pattern omsi_sparsity_pattern;

/* function prototypes for omsi_linear_system_t functions */
typedef int (*omsi_linear_system_t_get_x)(sim_data_t* data, omsi_vector_t* vector);


/**
 *
 */
typedef struct {
    unsigned int n_columns;
    unsigned int n_rows;
    double* tmp_vars;
    double* seed_vars;
    double* result_vars;

    equation_system_t* equations;
    int (*directionalDerivative) (equation_system_t *real_vars, omsi_analytical_jacobian *jacobian);
    //sparsity_pattern_t* sparsity_pattern;
}omsi_analytical_jacobian;

/**
 * general algebraic system
 */
typedef struct {
    equation_info_t* info;
    unsigned int n_iteration_vars;
    unsigned int n_input_vars;
    unsigned int n_inner_vars;

    unsigned int n_conditions;
    int *zerocrossing_indices;

    bool isLinear;      // linear system=true and non-linear system=false
    void *loop;        // points on array of omsi_linear_loop or omsi_non-linear_loop

    equation_system_t* equations;

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


/**
 *
 */
typedef struct _omsi_linear_system_t{

  int equation_index;       /* index for EQUATION_INFO */

  int n_iteration_vars;
  omsi_index_type *iteration_vars_indices;/* = {(OMSI_TYPE_DOUBLE, 4), (OMSI_TYPE_INTEGER,4)}; */

  int n_inputs_vars;
  omsi_index_type *input_vars_indices;/* = {(OMSI_TYPE_DOUBLE, 4), (OMSI_TYPE_INTEGER,4)}; */

  int n_inner_vars;
  omsi_index_type *inner_vars_indices;/* = {(OMSI_TYPE_DOUBLE, 4), (OMSI_TYPE_INTEGER,4)}; */

  int n_conditions;
  int *zc_index; /* index of zero crossings */

  /* easy driver */
  int (*get_a_matrix)(omsi_linear_system_t* linearSystem, void (*set_matrix_element)(int row, int col, double* val, void* data), void* data);
  int (*get_b_vector)(sim_data_t* data, omsi_vector_t* vector);

  /* advanced drivers */
  int (*get_sparsity_pattern)(omsi_sparsity_pattern* sparsity_pattern);
  int (*eval_residual)(sim_data_t* data, omsi_vector_t* x, omsi_vector_t* f, int ifail);
  int (*get_jacobian_column)(sim_data_t* data, omsi_vector_t* column); /* get symbolic directional derivatives */
} omsi_linear_system_t;


/**
 *
 */
void instatiate_linear_system();

/**
 *
 */
typedef struct {
  int n_system;
  int n_non_zeros;
  int n_conditions;
  int equation_index;       /* index for EQUATION_INFO */

  bool (*get_coditions)(sim_data_t* data, bool* vector);
  bool (*set_coditions)(sim_data_t* data, bool* vector);

  int (*get_x)(sim_data_t* data, omsi_vector_t* vector);
  int (*set_x)(sim_data_t* data, omsi_vector_t* vector);

  /* easy driver */
  int (*get_a_matrix)(sim_data_t* data, omsi_matrix_t* matrix);
  int (*get_b_vector)(sim_data_t* data, omsi_vector_t* vector);

  /* advanced drivers */
  int (*get_sparsity_pattern)(omsi_sparsity_pattern* sparsity_pattern);
  int (*eval_residual)(sim_data_t* data, omsi_vector_t* x, omsi_vector_t* f, int ifail);
  int (*get_jacobian_column)(sim_data_t* data, omsi_vector_t* column); /* get symbolic directional derivatives */
} omsi_nonlinear_system_t;

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
