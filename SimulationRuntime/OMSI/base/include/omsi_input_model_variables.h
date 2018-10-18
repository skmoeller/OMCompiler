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

omsi_status omsi_allocate_model_variables(omsi_t*                           omsu,
                                          const omsi_callback_functions*    functions);

omsi_status omsi_initialize_model_variables(omsi_t*                         omsu,
                                            const omsi_callback_functions*  functions,
                                            omsi_string                     instanceName);

omsi_status omsi_free_model_variables(omsi_t* omsu);


void *alignedMalloc(size_t required_bytes,
                    size_t alignment);

void alignedFree(void* p);

omsi_status omsi_get_boolean(omsi_t*                     omsu,
    const omsi_unsigned_int    vr[],
    omsi_unsigned_int          nvr,
    omsi_bool                  value[]);

omsi_status omsi_get_integer(omsi_t*                     omsu,
    const omsi_unsigned_int    vr[],
    omsi_unsigned_int          nvr,
    omsi_int                   value[]);

omsi_status omsi_get_real(omsi_t*                    omsu,
    const omsi_unsigned_int   vr[],
    omsi_unsigned_int         nvr,
    omsi_real                 value[]);

omsi_status omsi_get_string(omsi_t*                  omsu,
    const omsi_unsigned_int vr[],
    omsi_unsigned_int       nvr,
    omsi_string             value[]);

omsi_status omsi_set_boolean(omsi_t*                     omsu,
    const omsi_unsigned_int    vr[],
    omsi_unsigned_int          nvr,
    const omsi_bool            value[]);

omsi_status omsi_set_integer(omsi_t*                     omsu,
    const omsi_unsigned_int    vr[],
    omsi_unsigned_int          nvr,
    const omsi_int             value[]);

omsi_status omsi_set_real(omsi_t*                    omsu,
    const omsi_unsigned_int   vr[],
    omsi_unsigned_int         nvr,
    const omsi_real           value[]);

omsi_status omsi_set_string(omsi_t*                  omsu,
    const omsi_unsigned_int vr[],
    omsi_unsigned_int       nvr,
    const omsi_string       value[]);


omsi_bool model_variables_allocated(omsi_t*     omsu,
                                    omsi_string functionName);


omsi_real getReal (omsi_t*                  osu_data,
                   const omsi_unsigned_int  vr);

omsi_status setReal(omsi_t*                 osu_data,
                    const omsi_unsigned_int vr,
                    const omsi_real         value);

omsi_int getInteger (omsi_t*                    osu_data,
                     const omsi_unsigned_int    vr);

omsi_status setInteger(omsi_t*                  osu_data,
                       const omsi_unsigned_int  vr,
                       const omsi_int           value);

omsi_bool getBoolean (omsi_t*                  osu_data,
                      const omsi_unsigned_int   vr);

omsi_status setBoolean(omsi_t*                  osu_data,
                       const omsi_unsigned_int  vr,
                       const omsi_bool          value);

omsi_string getString (omsi_t*                  osu_data,
                       const omsi_unsigned_int  vr);

omsi_status setString(omsi_t*                  osu_data,
                      const omsi_unsigned_int   vr,
                      const omsi_string         value);

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif
#endif
