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

/*! \file omc_matrix.h
 */

#ifndef _OMC_MATRIX_H_
#define _OMC_MATRIX_H_


typedef enum {
    ROW_WISE,
    COLUMN_WISE
} omc_matrix_orientation;

typedef enum {
    DENSE_MATRIX,
    SPARSE_MATRIX
} omc_matrix_type;

typedef struct {
  void* data;
  omc_matrix_orientation orientation;
  omc_matrix_type type;
}omc_matrix;

/* memory management */
omc_matrix* allocate_matrix(unsigned int size_rows, unsigned int size_cols, int nnz, omc_matrix_orientation orientation, omc_matrix_type type);
void free_matrix(omc_matrix* A);
omc_matrix* copy_matrix(omc_matrix* A);

/* get and set functions */
double get_matrix_element(omc_matrix* A, int row, int col);
void set_matrix_element(omc_matrix* A, int row, int col, int nth, double value);

/* matrix operations */
void scale_matrix(omc_matrix* A, double scalar);
void set_zero_matrix(omc_matrix* A);

/* print functions */
void print_matrix(omc_matrix* A, const char* name, const int logLevel);


#endif
