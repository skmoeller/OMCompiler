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

/*! \file omc_matrix.c
 */
#include "omc_config.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "simulation_data.h"
#include "simulation/simulation_info_json.h"
#include "util/omc_error.h"
#include "util/varinfo.h"
#include "model_help.h"

#include "omc_matrix.h"
#include "omc_sparse_matrix.h"
#include "omc_math.h"

/**
 * Allocates memory for specific matrix and identifies the type of matrix.
 *
 * \param [in]     size_rows            Size of Rows in Matrix.
 * \param [in]     size_cols            Size of Columns in Matrix.
 * \param [in]     nnz                  Number of nonzero elements in Matrix.
 * \param [in]     orientation          Matrix initialization, Row wise or Column wise
 * \param [in]     type                 Dense or Sparse Matrix
 * \param [ref]    omc_matrix           Structure
 */
omc_matrix*
allocate_matrix(unsigned int size_rows, unsigned int size_cols, int nnz, omc_matrix_orientation orientation, omc_matrix_type type)
{
  omc_matrix* A = (omc_matrix*) malloc(sizeof(omc_matrix));

  switch (type){
    case DENSE_MATRIX:
      A->data = _omc_allocateMatrixData(size_rows, size_cols, orientation);
      A->orientation = orientation;
      A->type = type;
      break;
    case SPARSE_MATRIX:
      A->data = allocate_sparse_matrix(size_rows, size_cols, nnz, orientation);
      A->orientation = orientation;
      A->type = type;
      break;
    default:
      break;
    }
  return (A);
}

/**
 * Copies omc_matrix.
 *
 * \param [ref]     omc_sparse_matrix           Original Matrix.
 * \param [out]     omc_sparse_matrix           Copied Matrix.
 */
omc_matrix*
copy_matrix(omc_matrix* A)
{
  omc_matrix* mat = (omc_matrix*) malloc(sizeof(omc_matrix));
  mat->orientation = A->orientation;
  mat->type = A->type;
  switch (A->type){
    case DENSE_MATRIX:
      mat->data = _omc_copyMatrix((_omc_dense_matrix*)A->data);
      break;
    case SPARSE_MATRIX:
      mat->data = copy_sparse_matrix((omc_sparse_matrix*)A->data);
      break;
    default:
      break;
    }
  return(mat);
}

/**
 * Deallocates memory for specific matrix.
 *
 * \param [ref]    omc_matrix           Structure.
 */
void
free_matrix(omc_matrix* A)
{
  switch (A->type){
    case DENSE_MATRIX:
      _omc_deallocateMatrixData((_omc_dense_matrix*)A->data);
      break;
    case SPARSE_MATRIX:
      free_sparse_matrix((omc_sparse_matrix*)A->data);
      break;
    default:
      break;
    }
}

/**
 * Set all Elements in specific Matrix to Zero.
 *
 * \param [ref]     omc_matrix           Structure.
 * \param [out]     omc_matrix           Structure.
 */
void
set_zero_matrix(omc_matrix* A)
{
  switch (A->type){
    case DENSE_MATRIX:
      A->data = _omc_fillMatrix((_omc_dense_matrix*)A->data, 0.0);
      break;
    case SPARSE_MATRIX:
      A->data = set_zero_sparse_matrix((omc_sparse_matrix*)A->data);
      break;
    default:
      break;
    }
}

/**
 * Sets the (i,j) Element in the Matrix.
 *
 * \param [ref]     omc_matrix           Structure.
 * \param [in]      row                  Index Row.
 * \param [in]      col                  Index Column.
 * \param [in]      nth                  Position in Array.
 * \param [in]      value                Value that is set.
 * \param [ref]     omc_matrix           Structure.
 */
void
set_matrix_element(omc_matrix* A, int row, int col, int nth, double value)
{
  switch (A->type){
    case DENSE_MATRIX:
      _omc_setMatrixElement((_omc_dense_matrix*)A->data, row, col, value);
      break;
    case SPARSE_MATRIX:
      set_sparse_matrix_element((omc_sparse_matrix*)A->data, row, col, nth, value);
      break;
    default:
      break;
    }
}

/**
 * Gets the (i,j) Element in the Matrix.
 *
 * \param [ref]     omc_matrix           Structure.
 * \param [in]      row                  Index Row.
 * \param [in]      col                  Index Column.
 * \param [out]     double               Element A(i,j).
 */
double
get_matrix_element(omc_matrix* A, int row, int col)
{
  switch (A->type){
    case DENSE_MATRIX:
       return(_omc_getMatrixElement((_omc_dense_matrix*)A->data , row, col));
      break;
    case SPARSE_MATRIX:
      return(get_sparse_matrix_element((omc_sparse_matrix*)A->data, row, col));
      break;
    default:
      break;
    }
}

/**
 * Scales the Matrix with a constant Scaling Factor.
 *
 * \param [ref]     omc_matrix           Structure.
 * \param [in]      scalar               Scaling-Factor.
 * \param [ref]     omc_matrix           Structure.
 */
void
scale_matrix(omc_matrix* A, double scalar)
{
  switch (A->type){
    case DENSE_MATRIX:
      A->data = _omc_multiplyScalarMatrix((_omc_dense_matrix*)A->data, scalar);
      break;
    case SPARSE_MATRIX:
      A->data = scale_sparse_matrix((omc_sparse_matrix*)A->data, scalar);
      break;
    default:
      break;
    }
}

/**
 * Print the Matrix.
 *
 * \param [ref]     omc_matrix           Structure.
 */
void
print_matrix(omc_matrix* A, const char* name, const int logLevel)
{
  switch (A->type){
    case DENSE_MATRIX:
     _omc_printMatrix((_omc_dense_matrix*)A->data, name, logLevel);
      break;
    case SPARSE_MATRIX:
      print_sparse_matrix((omc_sparse_matrix*)A->data, logLevel);
      break;
    default:
      break;
    }
}
