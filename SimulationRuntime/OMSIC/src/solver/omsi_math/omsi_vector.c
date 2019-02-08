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

/*! \file omc_math.c
 */

#include "../../../include/math/omsi_vector.h"

#include <string.h> /* memcpy */
#include <math.h>


/*! \fn omsi_vector_t* _omsi_allocateVectorData(omsi_size_t size)
 *
 *  allocate omsi_vector_t and memory of size rows for data
 *
 *  \param [in]  [size] Number of elements
 */
omsi_vector_t* _omsi_allocateVectorData(const omsi_size_t size) {
  omsi_vector_t* vec = NULL;
  omsi_scalar_t* data = NULL;

  //assertStreamPrint(NULL, size > 0, "size needs to be greater zero");

  vec = (omsi_vector_t*) malloc(sizeof(omsi_vector_t));
  //assertStreamPrint(NULL, NULL != vec, "out of memory");

  data = (omsi_scalar_t*) malloc(size * sizeof(omsi_scalar_t));
  //assertStreamPrint(NULL, NULL != data, "out of memory");

  vec->size = size;
  vec->data = data;

  return vec;
}

/* TODO: change behaviour of all _omsi_allocate... functions to work with pointers.
 *      Use advantage of similar function behaving similar i.e. with _omsi_dealloc...
 * allocates memory for a given pointer to an OMSI vector of any size
 *

int _omsi_allocateVectorData(omsi_vector_t *vec, const omsi_size_t size) {

    vec = (omsi_vector_t*) malloc(sizeof(omsi_vector_t));
    //assertStreamPrint(NULL, NULL != vec, "out of memory");

    vec->data = (omsi_scalar_t*) malloc(size * sizeof(omsi_scalar_t));
    //assertStreamPrint(NULL, NULL != data, "out of memory");

    vec->size = size;

    return 0;
} */

/*! \fn void _omsi_deallocateVectorData(omsi_vector_t* vec)
 *
 *  free memory in data
 *
 *  \param [ref] [vec] !TODO: DESCRIBE ME!
 */
void _omsi_deallocateVectorData(omsi_vector_t* vec) {
  free(vec->data);
  free(vec);
}

/*! \fn omsi_vector_t* _omsi_createVector(omsi_size_t size, omsi_scalar_t* data)
 *
 *  creates a omsi_vector_t with a data of omsi_vector_t
 *
 *  \param [in]  [size] size of the vector
 *  \param [ref] [data] !TODO: DESCRIBE ME!
 */
omsi_vector_t* _omsi_createVector(const omsi_size_t size, omsi_scalar_t* data) {
  omsi_vector_t* vec = NULL;
  //assertStreamPrint(NULL, size > 0, "size needs to be greater zero");

  vec = (omsi_vector_t*) malloc(sizeof(omsi_vector_t));
  //assertStreamPrint(NULL, NULL != vec, "out of memory");

  vec->size = size;
  vec->data = data;

  return vec;
}

/*! \fn void _omsi_destroyVector(omsi_vector_t* vec)
 *
 *  free omsi_vector_t
 *
 *  \param [ref] [vec] !TODO: DESCRIBE ME!
 */
void _omsi_destroyVector(omsi_vector_t* vec) {
  free(vec);
}

/*! \fn void _omsi_copyVector(omsi_vector_t* dest, const omsi_vector_t* src)
 *
 *  creates a new omsi_vector_t by coping all data
 *
 *  \param [out] [dest] the destination vector, which need already be allocated
 *  \param [in]  [src]  the original vector
 */
void _omsi_copyVector(omsi_vector_t* dest, const omsi_vector_t* src)
{
  //assertStreamPrint(NULL, dest->size == src->size, "sizes of the vector need to be equal");
  memcpy(dest->data, src->data, sizeof(omsi_scalar_t) * dest->size);
}


/*! \fn omsi_scalar_t* _omsi_getVectorData(omsi_vector_t* vec)
 *
 *  get data of omsi_vector_t
 *
 *  \param [ref] [vec] !TODO: DESCRIBE ME!
 */
omsi_scalar_t* _omsi_getVectorData(omsi_vector_t* vec)
{
  return vec->data;
}

/*! \fn omsi_size_t _omsi_getVectorSize(omsi_vector_t* vec)
 *
 *  get size of omsi_vector_t
 *
 *  \param [ref] [vec] !TODO: DESCRIBE ME!
 */
omsi_size_t _omsi_getVectorSize(omsi_vector_t* vec)
{
  return vec->size;
}

/*! \fn omsi_scalar_t _omsi_getVectorElement(omsi_vector_t* vec, omsi_size_t i)
 *
 *  get i-th element of omsi_vector_t
 *
 *  \param [ref] [vec] !TODO: DESCRIBE ME!
 *  \param [in]  [i]   element
 */
omsi_scalar_t _omsi_getVectorElement(omsi_vector_t* vec, const omsi_size_t i)
{
  //assertStreamPrint(NULL, 0 <= i, "index out of bounds: %d", (int)i);
  //assertStreamPrint(NULL, i < vec->size, "omsi_vector_t size %d smaller than %d", (int)vec->size, (int)i);
  return vec->data[i];
}

/*! \fn void _omsi_setVectorElement(omsi_vector_t* vec, omsi_size_t i, omsi_scalar_t s)
 *
 *  set i-th element of omsi_vector_t
 *
 *  \param [ref] [vec] !TODO: DESCRIBE ME!
 *  \param [in]  [i]   element
 *  \param [in]  [s]   !TODO: DESCRIBE ME!
 */
void _omsi_setVectorElement(omsi_vector_t* vec, const omsi_size_t i, const omsi_scalar_t s)
{
  //assertStreamPrint(NULL, 0 <= i, "index out of bounds: %d", (int)i);
  //assertStreamPrint(NULL, i < vec->size, "omsi_vector_t size %d smaller than %d", (int)vec->size, (int)i);
  vec->data[i] = s;
}

/*! \fn omsi_scalar_t* _omsi_setVectorData(omsi_vector_t* vec, omsi_scalar_t* data)
 *
 *  replaces data of omsi_vector_t and return the old one
 *
 *  \param [ref] [vec]   !TODO: DESCRIBE ME!
 *  \param [ref] [data*] !TODO: DESCRIBE ME!
 */
omsi_scalar_t* _omsi_setVectorData(omsi_vector_t* vec, omsi_scalar_t* data)
{
  omsi_scalar_t* output = vec->data;
  vec->data = data;
  return output;
}


/*! \fn omsi_vector_t* _omsi_fillVector(omsi_vector_t* vec, omsi_scalar_t s)
 *
 *  fill all elements of omsi_vector_t by s
 *
 *  \param [ref] [vec] !TODO: DESCRIBE ME!
 *  \param [in]  [s]   !TODO: DESCRIBE ME!
 */
omsi_vector_t* _omsi_fillVector(omsi_vector_t* vec, omsi_scalar_t s)
{
  omsi_size_t i;

  //assertStreamPrint(NULL, NULL != vec->data, "omsi_vector_t data is NULL pointer");
  for (i = 0; i < vec->size; ++i)
  {
    vec->data[i] = s;
  }

  return vec;
}

/*! \fn omsi_vector_t* _omsi_negateVector(omsi_vector_t* vec)
 *
 *  negate all elements of omsi_vector_t
 *
 *  \param [ref] [omsi_vector_t] !TODO: DESCRIBE ME!
 */
omsi_vector_t* _omsi_negateVector(omsi_vector_t* vec)
{
  omsi_size_t i;

  //assertStreamPrint(NULL, NULL != vec->data, "omsi_vector_t data is NULL pointer");
  for (i = 0; i < vec->size; ++i)
  {
    vec->data[i] = -vec->data[i];
  }

  return vec;
}

/*! \fn omsi_vector_t* _omsi_multiplyScalarVector(omsi_vector_t* vec, omsi_scalar_t s)
 *
 *  multiply all elements of omsi_vector_t by s
 *
 *  \param [ref] [vec] !TODO: DESCRIBE ME!
 *  \param [in]  [s]   !TODO: DESCRIBE ME!
 */
omsi_vector_t* _omsi_multiplyScalarVector(omsi_vector_t* vec, omsi_scalar_t s)
{
  omsi_size_t i;

  //assertStreamPrint(NULL, NULL != vec->data, "omsi_vector_t data is NULL pointer");
  for (i = 0; i < vec->size; ++i)
  {
    vec->data[i] *= s;
  }

  return vec;
}

/*! \fn omsi_vector_t* _omsi_addVector(omsi_vector_t* dest, omsi_vector_t* src)
 *
 *  addition of two vectors to the first one
 *
 *  \param [ref] [dest] !TODO: DESCRIBE ME!
 *  \param [ref] [src]  !TODO: DESCRIBE ME!
 */
omsi_vector_t* _omsi_addVector(omsi_vector_t* dest, const omsi_vector_t* src)
{
  omsi_size_t i;
  //assertStreamPrint(NULL, dest->size == src->size,
  //    "Vectors have not the same size %d != %d", (int)dest->size, (int)src->size);
  //assertStreamPrint(NULL, NULL != dest->data, "vector1 data is NULL pointer");
  //assertStreamPrint(NULL, NULL != src->data, "vector2 data is NULL pointer");
  for (i = 0; i < dest->size; ++i) {
    dest->data[i] += src->data[i];
  }

  return dest;
}

/*! \fn omsi_vector_t* _omsi_subVector(omsi_vector_t* dest, const omsi_vector_t* src)
 *
 *  subtraction of two vectors to the first one
 *
 *  \param [ref] [dest] !TODO: DESCRIBE ME!
 *  \param [ref] [src]  !TODO: DESCRIBE ME!
 */
omsi_vector_t* _omsi_subVector(omsi_vector_t* dest, const omsi_vector_t* src)
{
  omsi_size_t i;
  //assertStreamPrint(NULL, src->size == dest->size, "Vectors have not the same size %d != %d", (int)src->size, (int)dest->size);
  //assertStreamPrint(NULL, NULL != dest->data, "vector1 data is NULL pointer");
  //assertStreamPrint(NULL, NULL != src->data, "vector2 data is NULL pointer");
  for (i = 0; i < dest->size; ++i)
  {
    dest->data[i] -= src->data[i];
  }

  return dest;
}

/*! \fn omsi_vector_t* _omsi_addVectorVector(omsi_vector_t dest, const omsi_vector_t* vec1, const omsi_vector_t* vec2)
 *
 *  addition of two vectors in a third one
 *
 *  \param [ref] [dest] !TODO: DESCRIBE ME!
 *  \param [ref] [vec1] !TODO: DESCRIBE ME!
 *  \param [ref] [vec2] !TODO: DESCRIBE ME!
 */
omsi_vector_t* _omsi_addVectorVector(omsi_vector_t* dest, const omsi_vector_t* vec1, const omsi_vector_t* vec2)
{
  omsi_size_t i;
  //assertStreamPrint(NULL, vec1->size == vec2->size && dest->size == vec1->size,
  //    "Vectors have not the same size %d != %d != %d", (int)dest->size, (int)vec1->size, (int)vec2->size);
  //assertStreamPrint(NULL, NULL != vec1->data, "vector1 data is NULL pointer");
  //assertStreamPrint(NULL, NULL != vec2->data, "vector2 data is NULL pointer");
  //assertStreamPrint(NULL, NULL != dest->data, "destination data is NULL pointer");
  for (i = 0; i < vec1->size; ++i) {
    dest->data[i] = vec1->data[i] + vec2->data[i];
  }

  return dest;
}

/*! \fn omsi_vector_t* _omsi_subVectorVector(omsi_vector_t* dest, const omsi_vector_t* vec1, const omsi_vector_t* vec2)
 *
 *  subtraction of vec2 from vec1 in a third one
 *
 *  \param [ref] [dest] !TODO: DESCRIBE ME!
 *  \param [ref] [vec1] !TODO: DESCRIBE ME!
 *  \param [ref] [vec2] !TODO: DESCRIBE ME!
 */
omsi_vector_t* _omsi_subVectorVector(omsi_vector_t* dest, const omsi_vector_t* vec1, const omsi_vector_t* vec2)
{
  omsi_size_t i;
  //assertStreamPrint(NULL, vec1->size == vec2->size && dest->size == vec1->size,
  //   "Vectors have not the same size %d != %d", (int)vec1->size, (int)vec2->size);
  //assertStreamPrint(NULL, NULL != vec1->data, "vector1 data is NULL pointer");
  //assertStreamPrint(NULL, NULL != vec2->data, "vector2 data is NULL pointer");
  //assertStreamPrint(NULL, NULL != dest->data, "destination data is NULL pointer");
  for (i = 0; i < vec1->size; ++i)
  {
    dest->data[i] = vec1->data[i] - vec2->data[i];
  }

  return dest;
}

/*! \fn omsi_scalar_t omsi_scalar_tProduct(omsi_vector_t* vec1, omsi_vector_t* vec2)
 *
 *  omsi_vector_t multiplication of two vectors
 *
 *  \param [ref] [vec1] !TODO: DESCRIBE ME!
 *  \param [ref] [vec2] !TODO: DESCRIBE ME!
 */
omsi_scalar_t omsi_scalar_tProduct(const omsi_vector_t* vec1, const omsi_vector_t* vec2)
{
  omsi_size_t i;
  omsi_size_t m = vec1->size, n = vec2->size;
  omsi_scalar_t result = 0;
  //assertStreamPrint(NULL, m == n, "Vectors size doesn't match to multiply %d != %d ", (int)m, (int)n);
  //assertStreamPrint(NULL, NULL != vec1->data, "vector1 data is NULL pointer");
  //assertStreamPrint(NULL, NULL != vec2->data, "vector2 data is NULL pointer");
  for (i = 0; i < n; ++i)
  {
    result += vec1->data[i] * vec2->data[i];
  }

  return result;
}

/*! \fn omsi_scalar_t _omsi_sumVector(omsi_vector_t* vec)
 *
 *  calculates the sum of all elements of the vector
 *
 *  \param [ref] [omsi_vector_t] !TODO: DESCRIBE ME!
 */
omsi_scalar_t _omsi_sumVector(const omsi_vector_t* vec) {
  omsi_size_t i;
  omsi_scalar_t sum = 0;
  //assertStreamPrint(NULL, NULL != vec->data, "vector data is NULL pointer");
  for (i = 0; i < vec->size; ++i)
  {
    sum += vec->data[i];
  }

  return sum;
}


/*! \fn void _omsi_printVector(omsi_vector_t* vec, char* name, int logLevel)
 *
 *  outputs the omsi_vector_t
 *
 *  \param [in]  [vec]      !TODO: DESCRIBE ME!
 *  \param [in]  [name]     !TODO: DESCRIBE ME!
 *  \param [in]  [logLevel] !TODO: DESCRIBE ME!
 */
/*
void _omsi_printVectorWithEquationInfo(omsi_vector_t* vec, const char* name, const int logLevel, EQUATION_INFO eqnInfo)
{
  omsi_size_t i;
  return;
  //if (!ACTIVE_STREAM(logLevel))
  //  return;

 // assertStreamPrint(NULL, NULL != vec->data, "Vector data is NULL pointer");

  //infoStreamPrint(logLevel, 1, "%s", name);
  //for (i = 0; i < vec->size; ++i)
  //{
  //  infoStreamPrint(logLevel, 0, "[%3d] %-40s = %20.12g",   (int)i+1, eqnInfo.vars[i], vec->data[i]);
  //}
  //messageClose(logLevel);
}*/

/*! \fn void _omsi_printVector(omsi_vector_t* vec, char* name, int logLevel)
 *
 *  outputs the omsi_vector_t
 *
 *  \param [in]  [vec]      !TODO: DESCRIBE ME!
 *  \param [in]  [name]     !TODO: DESCRIBE ME!
 *  \param [in]  [logLevel] !TODO: DESCRIBE ME!
 */
void _omsi_printVector(omsi_vector_t* vec, const char* name, const int logLevel)
{
  //omsi_size_t i;

  //if (!ACTIVE_STREAM(logLevel))
    return;

  //assertStreamPrint(NULL, NULL != vec->data, "Vector data is NULL pointer");

  //infoStreamPrint(logLevel, 1, "%s", name);
  //for (i = 0; i < vec->size; ++i)
  //{
  //  infoStreamPrint(logLevel, 0, "[%2d] %20.12g", (int)i+1, vec->data[i]);
  //}
  //messageClose(logLevel);
}


/*! \fn omsi_scalar_t _omsi_euclideanVectorNorm(omsi_vector_t* vec)
 *
 *  calculates the euclidean vector norm
 *
 *  \param [in]  [vec] !TODO: DESCRIBE ME!
 */
omsi_scalar_t _omsi_euclideanVectorNorm(const omsi_vector_t* vec)
{
  omsi_size_t i;
  omsi_scalar_t result = 0;
  //assertStreamPrint(NULL, vec->size > 0, "Vector size is greater than zero");
  //assertStreamPrint(NULL, NULL != vec->data, "Vector data is NULL pointer");
  for (i = 0; i < vec->size; ++i) {
    result += pow(fabs(vec->data[i]),2.0);
  }

  return sqrt(result);
}

/*! \fn omsi_scalar_t _omsi_maximumVectorNorm(omsi_vector_t* vec)
 *
 *  calculates the maximum vector norm
 *
 *  \param [ref] [vec] !TODO: DESCRIBE ME!
 */
omsi_scalar_t _omsi_maximumVectorNorm(const omsi_vector_t* vec)
{
  omsi_size_t i;
  omsi_scalar_t result, tmp;
  //assertStreamPrint(NULL, vec->size > 0, "Vector size is greater the zero");
  //assertStreamPrint(NULL, NULL != vec->data, "Vector data is NULL pointer");
  result = fabs(vec->data[0]);
  for (i = 1; i < vec->size; ++i)
  {
    tmp = fabs(vec->data[i]);
    if (result > tmp)
    {
      result = tmp;
    }
  }

  return result;
}
