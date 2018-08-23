#ifndef OMSI_CALLBACKS_H
#define OMSI_CALLBACKS_H

#ifdef __cplusplus
extern "C" {
#endif

int evaluate_ode(omsi_t);
int evaluate_outputs(omsi_t);
int evaluate_zerocrossings(omsi_t);
int evaluate_discrete_system(omsi_t);
int evaluate_bound_parameter(omsi_t);
int evaluate_intial_system(omsi_t);

/* ToDo: is this the right location for these definitions? */
/*
 * OMSI callback functions
 */
typedef void      (*omsi_callback_logger)           (const void*, omsi_string, omsi_status, omsi_string, omsi_string, ...);
typedef void*     (*omsi_callback_allocate_memory)  (omsi_unsigned_int, omsi_unsigned_int);
typedef void      (*omsi_callback_free_memory)      (void*);
typedef void      (*omsi_step_finished)             (void*, omsi_status);


typedef struct omsi_callback_functions{
    const omsi_callback_logger          logger;
    const omsi_callback_allocate_memory allocateMemory;
    const omsi_callback_free_memory     freeMemory;
    const omsi_step_finished            stepFinished;
    const void*                         componentEnvironment;
}omsi_callback_functions;



#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
