#ifndef _OSI_EQNS_SYSTEM_H
#define _OSI_EQNS_SYSTEM_H
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


/* forward some types */
struct sim_data_t;
struct omsi_vector_t;
struct omsi_matrix_t;
struct omsi_sparsity_pattern;


/**
 *
 */
struct omsi_linear_system_t
{
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
};
/**
 *
 */
void instatiate_linear_system();
/**
 *
 */
struct omsi_nonlinear_system_t
{
  int n_system;
  int n_non_zeros;
  int n_conditions;
  int equation_index;       /* index for EQUATION_INFO */

  bool (*get_coditions(sim_data_t* data, bool* vector));
  bool (*set_coditions(sim_data_t* data, bool* vector));

  int (*get_x)(sim_data_t* data, omsi_vector_t* vector);
  int (*set_x)(sim_data_t* data, omsi_vector_t* vector);

  /* easy driver */
  int (*get_a_matrix)(sim_data_t* data, omsi_matrix_t* matrix);
  int (*get_b_vector)(sim_data_t* data, omsi_vector_t* vector);

  /* advanced drivers */
  int (*get_sparsity_pattern)(omsi_sparsity_pattern* sparsity_pattern);
  int (*eval_residual)(sim_data_t* data, omsi_vector_t* x, omsi_vector_t* f, int ifail);
  int (*get_jacobian_column)(sim_data_t* data, omsi_vector_t* column); /* get symbolic directional derivatives */
};

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
