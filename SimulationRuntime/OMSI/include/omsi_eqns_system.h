#ifndef _OMSI_EQNS_SYSTEM_H
#define _OMSI_EQNS_SYSTEM_H

#include <stdbool.h>
#include "../../c/omsi/Solver/omsi_math/omsi_math.h"

#ifdef __cplusplus
extern "C" {
#endif


/* forward some types */
typedef struct omsi_sparsity_pattern omsi_sparsity_pattern;
//typedef struct omsi_vector_t omsi_vector_t;
//typedef struct omsi_matrix_t omsi_matrix_t;

/* function prototypes for omsi_linear_system_t functions */
typedef int (*omsi_linear_system_t_get_x)(sim_data_t* data, omsi_vector_t* vector);

/**
 *
 */
typedef struct {
  int n_system;
  int n_non_zeros;
  int n_conditions;
  int equation_index;       /* index for EQUATION_INFO */

  bool (*get_coditions)(sim_data_t* data, bool* vector);
  bool (*set_coditions)( sim_data_t* data, bool* vector);

  //int (*get_x)(sim_data_t* data, omsi_vector_t* vector);
  omsi_linear_system_get_x get_x;
  int (*set_x)(sim_data_t* data, omsi_vector_t* vector);

  /* easy driver */
  int (*get_a_matrix)(sim_data_t* data, omsi_matrix_t* matrix);
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
