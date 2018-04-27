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

#ifndef OSIMATRIX_H_
#define OSIMATRIX_H_

#include "omsi_math.h"
#include "omsi_vector.h"

typedef struct {
  omsi_size_t rows;
  omsi_size_t cols;
  omsi_scalar_t *data;
} omsi_matrix_t;

/* memory management matrix */
omsi_matrix_t* _omsi_allocateMatrixData(const omsi_size_t rows, const omsi_size_t cols);
void _omsi_deallocateMatrixData(omsi_matrix_t* mat);
omsi_matrix_t* _omsi_createMatrix(const omsi_size_t rows, const omsi_size_t cols, omsi_scalar_t* data);
void _omsi_destroyMatrix(omsi_matrix_t* mat);
omsi_matrix_t* _omsi_copyMatrix(omsi_matrix_t* mat);

/* get and set matrix */
omsi_scalar_t* _omsi_getMatrixData(omsi_matrix_t* mat);
omsi_size_t _omsi_getMatrixRows(omsi_matrix_t* mat);
omsi_size_t _omsi_getMatrixCols(omsi_matrix_t* mat);
omsi_size_t _omsi_getMatrixSize(omsi_matrix_t* mat);
omsi_scalar_t _omsi_getMatrixElement(omsi_matrix_t* mat, const omsi_size_t i, const omsi_size_t j);
void _omsi_setMatrixElement(omsi_matrix_t* mat, const omsi_size_t i, const omsi_size_t j, omsi_scalar_t s);
omsi_scalar_t* _omsi_setMatrixData(omsi_matrix_t* mat, omsi_scalar_t* data);
omsi_matrix_t* _omsi_addMatrixColumn(omsi_vector_t *vec, double scalar, omsi_matrix_t *mat, int columnIndex);

/* matrix operations */
omsi_matrix_t* _omsi_fillMatrix(omsi_matrix_t* mat, omsi_scalar_t s);
omsi_matrix_t* _omsi_fillIndentityMatrix(omsi_matrix_t* mat);
omsi_matrix_t* _omsi_negateMatrix(omsi_matrix_t* mat);

omsi_matrix_t* _omsi_multiplyScalarMatrix(omsi_matrix_t* mat, omsi_scalar_t s);
omsi_matrix_t* _omsi_addMatrixMatrix(omsi_matrix_t* mat1, omsi_matrix_t* mat2);
omsi_matrix_t* _omsi_subtractMatrixMatrix(omsi_matrix_t* mat1, omsi_matrix_t* mat2);
omsi_matrix_t* _omsi_multiplyMatrixMatrix(omsi_matrix_t* mat1, omsi_matrix_t* mat2);

/* print functions */
void _omsi_printMatrix(omsi_matrix_t* mat, const char* name, const int logLevel);

#endif
