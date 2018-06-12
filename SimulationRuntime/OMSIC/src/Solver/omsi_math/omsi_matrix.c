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

/*! \file omsi_matrix.c
 */

#include "../../../include/math/omsi_matrix.h"


/*! \fn omsi_matrix_t* _omsi_allocateMatrixData(omsi_size_t rows, omsi_size_t cols)
 *
 *  allocate omsi_matrix_t and memory of size rows*cols for data
 *
 *  \param [in]  [rows] Number of rows
 *  \param [in]  [cols] Number of cols
 */
omsi_matrix_t* _omsi_allocateMatrixData(const omsi_size_t rows, const omsi_size_t cols)
{
  omsi_matrix_t* mat = NULL;
  omsi_scalar_t* data = NULL;
  //assertStreamPrint(NULL, rows > 0, "size of rows need greater zero");
  //assertStreamPrint(NULL, cols > 0, "size of cols need greater zero");

  mat = (omsi_matrix_t*) malloc(sizeof(omsi_matrix_t));
  //assertStreamPrint(NULL, NULL != mat, "out of memory");

  data = (omsi_scalar_t*) malloc(rows * cols * sizeof(omsi_scalar_t));
  //assertStreamPrint(NULL, NULL != mat, "data out of memory");

  mat->rows = rows;
  mat->cols = cols;
  mat->data = data;

  return mat;
}

/*! \fn void _omsi_deallocateMatrixData(omsi_matrix_t* mat)
 *
 *  free memory in data
 *
 *  \param [ref] [mat] !TODO: DESCRIBE ME!
 */
void _omsi_deallocateMatrixData(omsi_matrix_t* mat)
{
  free(mat->data);
  free(mat);
}

/*! \fn omsi_matrix_t* _omsi_createMatrix(omsi_size_t rows, omsi_size_t cols, omsi_scalar_t* data)
 *
 *  creates a omsi_matrix_t with a data of omsi_matrix_t
 *
 *  \param [in]  [rows] Number of rows
 *  \param [in]  [cols] Number of cols
 *  \param [ref] [data] !TODO: DESCRIBE ME!
 */
omsi_matrix_t* _omsi_createMatrix(const omsi_size_t rows, const omsi_size_t cols, omsi_scalar_t* data)
{
  omsi_matrix_t* mat = NULL;
  //assertStreamPrint(NULL, rows > 0, "size of rows need greater zero");
  //assertStreamPrint(NULL, cols > 0, "size of cols need greater zero");

  mat = (omsi_matrix_t*) malloc(sizeof(omsi_matrix_t));
  //assertStreamPrint(NULL, NULL != mat, "out of memory");

  mat->rows = rows;
  mat->cols = cols;
  mat->data = data;

  return mat;
}

/*! \fn void _omsi_destroyMatrix(omsi_matrix_t* mat)
 *
 *  free omsi_matrix_t
 *
 *  \param [ref] [mat] !TODO: DESCRIBE ME!
 */
void _omsi_destroyMatrix(omsi_matrix_t* mat)
{
  free(mat);
}

/*! \fn omsi_matrix_t* _omsi_copyMatrix(omsi_matrix_t* mat1)
 *
 *  creates a new omsi_matrix_t by coping all data
 *
 *  \param [in]  [mat1] the original matrix
 */
omsi_matrix_t* _omsi_copyMatrix(omsi_matrix_t* mat1)
{
  omsi_matrix_t* mat = _omsi_allocateMatrixData(mat1->rows, mat1->cols);
  memcpy(mat->data, mat1->data, sizeof(omsi_scalar_t) * _omsi_getMatrixSize(mat1));
  return mat;
}


/*! \fn omsi_scalar_t* _omsi_getMatrixData(omsi_matrix_t* mat)
 *
 *  get data of omsi_matrix_t
 *
 *  \param [ref] [mat] !TODO: DESCRIBE ME!
 */
omsi_scalar_t* _omsi_getMatrixData(omsi_matrix_t* mat)
{
  return mat->data;
}

/*! \fn omsi_size_t _omsi_getMatrixRows(omsi_matrix_t* mat)
 *
 *  get rows size of omsi_matrix_t
 *
 *  \param [ref] [mat] !TODO: DESCRIBE ME!
 */
omsi_size_t _omsi_getMatrixRows(omsi_matrix_t* mat)
{
  return mat->rows;
}

/*! \fn omsi_size_t _omsi_getMatrixCols(omsi_matrix_t* mat)
 *
 *  get cols size of omsi_matrix_t
 *
 *  \param [ref] [mat] !TODO: DESCRIBE ME!
 */
omsi_size_t _omsi_getMatrixCols(omsi_matrix_t* mat)
{
  return mat->cols;
}

/*! \fn omsi_size_t _omsi_getMatrixSize(omsi_matrix_t* mat)
 *
 *  get size of omsi_matrix_t
 *
 *  \param [ref] [mat] !TODO: DESCRIBE ME!
 */
omsi_size_t _omsi_getMatrixSize(omsi_matrix_t* mat)
{
  return mat->rows * mat->cols;
}

/*! \fn omsi_scalar_t _omsi_getMatrixElement(omsi_matrix_t* mat, omsi_size_t i, omsi_size_t j)
 *
 *  get (i,j)-th element of omsi_matrix_t
 *
 *  \param [ref] [omsi_matrix_t] !TODO: DESCRIBE ME!
 *  \param [in]  [omsi_size_t]   rows
 *  \param [in]  [omsi_size_t]   cols
 */
omsi_scalar_t _omsi_getMatrixElement(omsi_matrix_t* mat, const omsi_size_t i, const omsi_size_t j)
{
  //assertStreamPrint(NULL, 0 <= i, "index i out of bounds: %d", (int)i);
  //assertStreamPrint(NULL, 0 <= j, "index j out of bounds: %d", (int)j);
  //assertStreamPrint(NULL, i < mat->rows, "omsi_matrix_t rows(%d) too small for %d", (int)mat->rows, (int)i);
  //assertStreamPrint(NULL, j < mat->cols, "omsi_matrix_t cols(%d) too small for %d", (int)mat->cols, (int)j);
  return mat->data[i + j * mat->cols];
}

/*! \fn void _omsi_setMatrixElement(omsi_matrix_t* mat, omsi_size_t i, omsi_size_t j, omsi_scalar_t s)
 *
 *  set i-th element of omsi_matrix_t
 *
 *  \param [ref] [mat] !TODO: DESCRIBE ME!
 *  \param [in]  [i]   rows
 *  \param [in]  [j]   cols
 *  \param [in]  [s]   !TODO: DESCRIBE ME!
 */
void _omsi_setMatrixElement(omsi_matrix_t* mat, const omsi_size_t i, const omsi_size_t j, omsi_scalar_t s)
{
  //assertStreamPrint(NULL, i < mat->rows, "omsi_matrix_t rows(%d) too small for %d", (int)mat->rows, (int)i);
  //assertStreamPrint(NULL, j < mat->cols, "omsi_matrix_t cols(%d) too small for %d", (int)mat->cols, (int)j);
  mat->data[i + j * mat->cols] = s;
}

/*! \fn omsi_scalar_t* _omsi_setMatrixData(omsi_matrix_t* mat, omsi_scalar_t* data)
 *
 *  get data of omsi_matrix_t
 *
 *  \param [ref] [mat]  !TODO: DESCRIBE ME!
 *  \param [in]  [data] !TODO: DESCRIBE ME!
 */
omsi_scalar_t* _omsi_setMatrixData(omsi_matrix_t* mat, omsi_scalar_t* data)
{
  omsi_scalar_t* output = mat->data;
  mat->data = data;
  return output;
}

/*! \fn omsi_matrix_t* _omsi_addMatrixColumn(int n, omsi_vector_t *b, double scalar, omsi_matrix_t *A, double *columnAddress)
 *
 *  add a column to a Matrix at a specific adress. you also have to set a scalar for the column (scalar * column)!
 *  default value for the scalar is 1.
 *
 *  \param [ref] [mat]  !TODO: DESCRIBE ME!
 *  \param [in]  [data] !TODO: DESCRIBE ME!
 */
omsi_matrix_t* _omsi_addMatrixColumn(omsi_vector_t *vec, double scalar, omsi_matrix_t *mat, int columnIndex)
{
  omsi_size_t i;
  for (i=columnIndex;i<vec->size+columnIndex;i++)
	  mat->data[i] = scalar*vec->data[i-columnIndex];
  return mat;
}

/*! \fn omsi_matrix_t* _omsi_fillMatrix(omsi_matrix_t* mat, omsi_scalar_t s)
 *
 *  fill all elements of omsi_matrix_t by s
 *
 *  \param [ref] [mat] !TODO: DESCRIBE ME!
 *  \param [in]  [s]   !TODO: DESCRIBE ME!
 */
omsi_matrix_t* _omsi_fillMatrix(omsi_matrix_t* mat, omsi_scalar_t s) {
  omsi_size_t i;
  omsi_size_t size = mat->rows * mat->cols;
  //assertStreamPrint(NULL, NULL != mat->data, "omsi_matrix_t data is NULL pointer");
  for (i = 0; i < size; ++i) {
    mat->data[i] = s;
  }

  return mat;
}

/*! \fn omsi_matrix_t* _omsi_fillIndentityMatrix(omsi_matrix_t* mat)
 *
 *  fill identity omsi_matrix_t
 *
 *  \param [ref] [mat] !TODO: DESCRIBE ME!
 */
omsi_matrix_t* _omsi_fillIndentityMatrix(omsi_matrix_t* mat)
{
  omsi_size_t i, min;
  //assertStreamPrint(NULL, NULL != mat->data, "omsi_matrix_t data is NULL pointer");
  mat = _omsi_fillMatrix(mat, 0);
  min = mat->rows <= mat->cols ? mat->rows : mat->cols;
  for (i = 0; i < min; ++i)
  {
    _omsi_setMatrixElement(mat, i, i, 1.0);
  }

  return mat;
}

/*! \fn omsi_matrix_t* _omsi_negateMatrix(omsi_matrix_t* mat)
 *
 *  negate all elements of omsi_matrix_t
 *
 *  \param [ref] [mat] !TODO: DESCRIBE ME!
 */
omsi_matrix_t* _omsi_negateMatrix(omsi_matrix_t* mat)
{
  omsi_size_t i, size = mat->rows * mat->cols;
  //assertStreamPrint(NULL, NULL != mat->data, "omsi_matrix_t data is NULL pointer");
  for (i = 0; i < size; ++i) {
    mat->data[i] = -mat->data[i];
  }

  return mat;
}

/*! \fn omsi_matrix_t* _omsi_multiplyScalarMatrix(omsi_matrix_t* mat, omsi_scalar_t s)
 *
 *  multiply all elements of omsi_matrix_t by s
 *
 *  \param [ref] [mat] !TODO: DESCRIBE ME!
 *  \param [in]  [s]   !TODO: DESCRIBE ME!
 */
omsi_matrix_t* _omsi_multiplyScalarMatrix(omsi_matrix_t* mat, omsi_scalar_t s)
{
  omsi_size_t i, size = mat->rows * mat->cols;
  //assertStreamPrint(NULL, NULL != mat->data, "omsi_matrix_t data is NULL pointer");
  for (i = 0; i < size; ++i)
  {
    mat->data[i] *= s;
  }

  return mat;
}

/*! \fn omsi_matrix_t* _omsi_addMatrixMatrix(omsi_matrix_t* mat1, omsi_matrix_t* mat2)
 *
 *  addition of two matrixs to the first one
 *
 *  \param [ref] [mat1] !TODO: DESCRIBE ME!
 *  \param [ref] [mat2] !TODO: DESCRIBE ME!
 */
omsi_matrix_t* _omsi_addMatrixMatrix(omsi_matrix_t* mat1, omsi_matrix_t* mat2)
{
  omsi_size_t i, j;
  //assertStreamPrint(NULL, mat1->rows == mat2->rows && mat1->cols == mat2->cols,
  //    "matrixes have not the same size ((%d,%d)!=(%d,%d))",
  //    (int)mat1->rows, (int)mat1->cols, (int)mat2->rows, (int)mat2->cols);
  //assertStreamPrint(NULL, NULL != mat1->data, "matrix1 data is NULL pointer");
  //assertStreamPrint(NULL, NULL != mat2->data, "matrix2 data is NULL pointer");
  for (i = 0; i < mat1->rows; ++i) {
    for (j = 0; j < mat1->cols; ++j) {
      _omsi_setMatrixElement(mat1, i, j,
          _omsi_getMatrixElement(mat1, i, j)
              + _omsi_getMatrixElement(mat2, i, j));
    }
  }

  return mat1;
}

/*! \fn omsi_matrix_t* _omsi_subtractMatrixMatrix(omsi_matrix_t* mat1, omsi_matrix_t* mat2)
 *
 *  subtraction of two matrixs to the first one
 *
 *  \param [ref] [mat1] !TODO: DESCRIBE ME!
 *  \param [ref] [mat2] !TODO: DESCRIBE ME!
 */
omsi_matrix_t* _omsi_subtractMatrixMatrix(omsi_matrix_t* mat1, omsi_matrix_t* mat2)
{
  omsi_size_t i, j;
  //assertStreamPrint(NULL, mat1->rows == mat2->rows && mat1->cols == mat2->cols,
  //    "matrixes have not the same size ((%d,%d)!=(%d,%d))",
  //    (int)mat1->rows, (int)mat1->cols, (int)mat2->rows, (int)mat2->cols);
  //assertStreamPrint(NULL, NULL != mat1->data, "matrix1 data is NULL pointer");
  //assertStreamPrint(NULL, NULL != mat2->data, "matrix2 data is NULL pointer");
  for (i = 0; i < mat1->rows; ++i) {
    for (j = 0; j < mat1->cols; ++j) {
      _omsi_setMatrixElement(mat1, i, j,
          _omsi_getMatrixElement(mat1, i, j)
              - _omsi_getMatrixElement(mat2, i, j));
    }
  }

  return mat1;
}

/*! \fn omsi_scalar_t* _omsi_multiplyMatrixMatrix(omsi_matrix_t* mat1, omsi_matrix_t* mat2)
 *
 *  omsi_matrix_t multiplication of two matrixes into matrix one
 *
 *  \param [ref] [mat1] !TODO: DESCRIBE ME!
 *  \param [ref] [mat2] !TODO: DESCRIBE ME!
 */
omsi_matrix_t* _omsi_multiplyMatrixMatrix(omsi_matrix_t* mat1, omsi_matrix_t* mat2)
{
  omsi_size_t i, j, k;
  omsi_size_t l = mat1->rows, m = mat1->cols, n = mat2->cols;
  //assertStreamPrint(NULL, mat1->cols == mat2->rows,
  //    "matrixes size doesn't match to multiply"
  //        "(%d!=%d)", (int)mat1->cols, (int)mat2->rows);
  //assertStreamPrint(NULL, NULL != mat1->data, "matrix1 data is NULL pointer");
  //assertStreamPrint(NULL, NULL != mat2->data, "matrix2 data is NULL pointer");
  for (i = 0; i < l; ++i) {
    for (j = 0; j < n; ++j) {
      for (k = 0; k < m; ++k) {
        _omsi_setMatrixElement(mat1, i, j,
            _omsi_getMatrixElement(mat1, i, k)
                * _omsi_getMatrixElement(mat2, k, j));
      }
    }
  }

  return mat1;
}

/*! \fn void _omsi_printMatrix(omsi_matrix_t* mat, char* name, int logLevel)
 *
 *  outputs the omsi_matrix_t
 *
 *  \param [in]  [mat]      !TODO: DESCRIBE ME!
 *  \param [in]  [name]     !TODO: DESCRIBE ME!
 *  \param [in]  [logLevel] !TODO: DESCRIBE ME!
 */
void _omsi_printMatrix(omsi_matrix_t* mat, const char* name, const int logLevel) {
 // if (ACTIVE_STREAM(logLevel))
  //{
    omsi_size_t i, j;
    char *buffer = (char*)malloc(sizeof(char)*mat->cols*20);

    //assertStreamPrint(NULL, NULL != mat->data, "matrix data is NULL pointer");

    //infoStreamPrint(logLevel, 1, "%s", name);
    for (i = 0; i < mat->rows; ++i)
    {
      buffer[0] = 0;
      for (j = 0; j < mat->cols; ++j)
      {
        sprintf(buffer, "%s%10g ", buffer, _omsi_getMatrixElement(mat, i, j));
      }
     // infoStreamPrint(logLevel, 0, "%s", buffer);
    }
    //messageClose(logLevel);
    free(buffer);
//  }
}

