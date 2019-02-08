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

/*! \file linearSolverLapack.h
 */

#ifndef _LINEARSOLVERTOTALPIVOT_INTERFACE_H_
#define _LINEARSOLVERTOTALPIVOT_INTERFACE_H_

//#include "simulation/simulation_info_json.h"
//#include "util/omc_error.h"
//#include "util/varinfo.h"

#include <math.h>
#include <stdlib.h>
#include <string.h> /* memcpy */

#include "../solver/TotalPivot.h"
#include "math/omsi_matrix.h"
#include "math/omsi_vector.h"

#include "math/omsi_math.h"
#include "omsi.h"
#include "omsi_eqns_system.h"
#include "util/rtclock.h"


typedef struct DATA_TOTALPIVOT
{
  /* memory for linear system */
  omsi_vector_t* x;
  omsi_vector_t* x_tmp;
  omsi_vector_t* b;
  omsi_matrix_t* Ab;


  /* used for pivot strategy */
  int* indRow;
  int* indCol;

  rtclock_t timeClock;             /* time clock */

} DATA_TOTALPIVOT;

int allocateTotalPivotData(int size, void** data);
int freeTotalPivotData(void** data);
int solveTotalPivot(DATA_TOTALPIVOT* pivotData, omsi_t *omsiData, omsi_linear_system_t *linearSystem);

#endif
