#ifndef OMSU_INPUT_MODEL_VARIABLES_H
#define OMSU_INPUT_MODEL_VARIABLES_H

#include <omsi.h>
#include <omsi_callbacks.h>
#include <omsi_global.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>



/* public function prototypes */

omsi_status omsi_allocate_model_variables(omsi_t*                           omsu,
                                          omsi_string                       filename,
                                          omsi_string                       fmuGUID,
                                          omsi_string                       instanceName,
                                          const omsi_callback_functions*    functions);


omsi_status omsi_free_model_variables(omsi_t*                           omsu,
                                      omsi_string                       filename,
                                      omsi_string                       fmuGUID,
                                      omsi_string                       instanceName,
                                      const omsi_callback_functions*    functions);



void *alignedMalloc(size_t required_bytes, size_t alignment);
void alignedFree(void* p);

#endif
