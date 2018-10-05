/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THE BSD NEW LICENSE OR THE
 * GPL VERSION 3 LICENSE OR THE OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs: http://www.openmodelica.org or
 * http://www.ida.liu.se/projects/OpenModelica, and in the OpenModelica
 * distribution. GNU version 3 is obtained from:
 * http://www.gnu.org/copyleft/gpl.html. The New BSD License is obtained from:
 * http://www.opensource.org/licenses/BSD-3-Clause.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, EXCEPT AS
 * EXPRESSLY SET FORTH IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE
 * CONDITIONS OF OSMC-PL.
 *
 */

/*
 * This file defines functions for the FMI model exchange used via the
 * OpenModelica Simulation Interface (OMSI).
 */

#ifndef OMSI_ME_H
#define OMSI_API_FUNC_H

/* OpenModelica Simulation Unit / OpenModelica Simulation Interface */
#include <omsu_initialization.h>
#include <omsu_common.h>
#include <omsu_continuous_simulation.h>
#include <omsu_event_simulation.h>
#include <omsu_getters_and_setters.h>
#include <omsu_utils.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes */
fmi2Status fmi2EventUpdate(fmi2Component c, fmi2EventInfo* eventInfo);

/* unsupported functions */

/*! \fn omsi_free_fmu_state
 *
 *  This function is not supported.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_free_fmu_state(fmi2Component c, fmi2FMUstate* FMUstate);

/*! \fn omsi_serialized_fmu_state_size
 *
 *  This function is not supported.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_serialized_fmu_state_size(fmi2Component c, fmi2FMUstate FMUstate,
                                          size_t* size);

/*! \fn omsi_serialize_fmu_state
 *
 *  This function is not supported.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_serialize_fmu_state(fmi2Component c, fmi2FMUstate FMUstate,
                                    fmi2Byte serializedState[], size_t size);

/*! \fn omsi_de_serialize_fmu_state
 *
 *  This function is not supported.
 *
 *  \param [ref] [data]
 */
fmi2Status omsi_de_serialize_fmu_state(fmi2Component c,
                                       const fmi2Byte serializedState[],
                                       size_t size, fmi2FMUstate* FMUstate);


#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
