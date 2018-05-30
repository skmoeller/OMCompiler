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

#ifndef _OMSI_JACOBIAN_H
#define _OMSI_JACOBIAN_H

#include "../../c/omsi/Solver/omsi_math/omsi_math.h"
#include "../../c/omsi/Solver/omsi_math/omsi_vector.h"
#include "../../c/omsi/Solver/omsi_math/omsi_matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

struct omsi_sparsity_pattern
{
  unsigned int  number_non_zeros;
  unsigned int* colindex;
  unsigned int* rowindex;
};

typedef struct {
  int row;
  int col;
  double val;
} omsi_triplet;

typedef struct {
  int nnz;
  int cols;
  int rows;
  omsi_triplet* triplet;
} omsi_triplet_matrix;

void set_omsi_matrix(int row, int col, double* val, void* input_data);
//{                                                         //ToDo: put in C file
//  omsi_matrix_t* A = (omsi_matrix_t*) input_data;
//  A->data[row+col*A->rows] = *val;
//}

void set_omsi_triplet_matrix(int row, int col, double* val, void* data);
//{
//  omsi_triplet_matrix* A = (omsi_triplet_matrix*) data;
//
//  A->data[row+col*A->rows] = *val;
//}


#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
