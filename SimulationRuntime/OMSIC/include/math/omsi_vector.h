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

#ifndef OSIVECTOR_H_
#define OSIVECTOR_H_

#include "omsi_math.h"

typedef struct {
  omsi_size_t size;
  omsi_scalar_t *data;
} omsi_vector_t;

/* memory management vector */
omsi_vector_t* _omsi_allocateVectorData(const omsi_size_t size);
void _omsi_deallocateVectorData(omsi_vector_t* vec);
omsi_vector_t* _omsi_createVector(const omsi_size_t size, omsi_scalar_t* data);
void _omsi_destroyVector(omsi_vector_t* vec);
void _omsi_copyVector(omsi_vector_t* dest, const omsi_vector_t* src);

/* get and set vector */
omsi_scalar_t* _omsi_getVectorData(omsi_vector_t* vec);
omsi_size_t _omsi_getVectorSize(omsi_vector_t* vec);
omsi_scalar_t _omsi_getVectorElement(omsi_vector_t* vec, const omsi_size_t i);
void _omsi_setVectorElement(omsi_vector_t* vec, const omsi_size_t i, omsi_scalar_t s);
omsi_scalar_t* _omsi_setVectorData(omsi_vector_t* vec, omsi_scalar_t* data);

/* vector operations */
omsi_vector_t* _omsi_fillVector(omsi_vector_t* vec, omsi_scalar_t s);
omsi_vector_t* _omsi_negateVector(omsi_vector_t* vec);

omsi_vector_t* _omsi_multiplyScalarVector(omsi_vector_t* vec, omsi_scalar_t s);
omsi_vector_t* _omsi_addVector(omsi_vector_t* vec1, const omsi_vector_t* vec2);
omsi_vector_t* _omsi_subVector(omsi_vector_t* vec1, const omsi_vector_t* vec2);
omsi_vector_t* _omsi_addVectorVector(omsi_vector_t* dest, const omsi_vector_t* vec1, const omsi_vector_t* vec2);
omsi_vector_t* _omsi_subVectorVector(omsi_vector_t* dest, const omsi_vector_t* vec1, const omsi_vector_t* vec2);
omsi_scalar_t omsi_scalar_tProduct(const omsi_vector_t* vec1, const omsi_vector_t* vec2);
omsi_scalar_t _omsi_sumVector(const omsi_vector_t* vec);


/* print functions */
//void _omsi_printVectorWithEquationInfo(omsi_vector_t* vec, const char* name, const int logLevel, EQUATION_INFO eqnInfo);
void _omsi_printVector(omsi_vector_t* vec, const char* name, const int logLevel);

/* norm functions */
omsi_scalar_t _omsi_euclideanVectorNorm(const omsi_vector_t* vec);
omsi_scalar_t _omsi_maximumVectorNorm(const omsi_vector_t* vec);

#endif
