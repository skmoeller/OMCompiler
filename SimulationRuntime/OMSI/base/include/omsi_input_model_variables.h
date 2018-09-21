#ifndef OMSU_INPUT_MODEL_VARIABLES_H
#define OMSU_INPUT_MODEL_VARIABLES_H

#include <omsi.h>
#include <omsi_callbacks.h>
#include <omsi_global.h>
#include <omsi_utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* public function prototypes */

omsi_int omsi_allocate_model_variables(omsi_t*                           omsu,
                                       const omsi_callback_functions*    functions);

omsi_int omsi_initialize_model_variables(omsi_t* omsu, const omsi_callback_functions*  functions, omsi_string instanceName);



omsi_int omsi_free_model_variables(omsi_t* omsu);


void *alignedMalloc(size_t required_bytes, size_t alignment);
void alignedFree(void* p);

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif
#endif
