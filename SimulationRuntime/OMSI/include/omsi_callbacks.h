#ifndef OIS_CALLBACKS_H
#define OIS_CALLBACKS_H

#ifdef __cplusplus
extern "C" {
#endif

int evaluate_ode(omsi_t);
int evaluate_outputs(omsi_t);
int evaluate_zerocrossings(omsi_t);
int evaluate_discrete_system(omsi_t);
int evaluate_bound_parameter(omsi_t);
int evaluate_intial_system(omsi_t);

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif