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

/*! \file nonlinearSolverNewton.h
 */

#ifndef _NONLINEARSOLVERNEWTON_H_
#define _NONLINEARSOLVERNEWTON_H_

#include <math.h>
#include <stdlib.h>
#include <string.h> /* memcpy */

#include "../solver/newtonIteration.h"
#include "math/omsi_matrix.h"
#include "math/omsi_vector.h"

#include "math/omsi_math.h"
#include "omsi.h"
#include "omsi_eqns_system.h"
#include "util/rtclock.h"

/*
#include "simulation/simulation_info_json.h"
#include "util/omc_error.h"
#include "util/varinfo.h"
#include "model_help.h"

#include "nonlinearSystem.h"



#include "external_input.h"
*/
#ifdef __cplusplus
extern "C" {
#endif

//typedef struct
//{
//  void* data;
//  threadData_t *threadData;
//  int sysNumber;
//} DATA_USER;

int solveNewton(DATA_NEWTON* newtonData, omsi_t *omsiData, omsi_nonlinear_system_t* nonlinearSystem);

#ifdef __cplusplus
}
#endif

#endif
