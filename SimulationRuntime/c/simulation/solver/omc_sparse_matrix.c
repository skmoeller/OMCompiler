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

/*! \file omc_sparse_matrix.c
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

#include "omc_sparse_matrix.h"
#include "omc_matrix.h"

/**
 * Allocates memory for omc_sparse matrix, depending on its orientation.
 *
 * \param [in]     size_rows            Size of Rows in Matrix.
 * \param [in]     size_cols            Size of Columns in Matrix.
 * \param [in]     nnz                  Number of nonzero elements in Matrix.
 * \param [in]     orientation          Matrix initialization, Row wise or Column wise
 * \return         omc_sparse_matrix    Structure
 */
omc_sparse_matrix*
allocate_sparse_matrix(unsigned int size_rows, unsigned int size_cols, int nnz, omc_matrix_orientation orientation)
{
  omc_sparse_matrix* A = (omc_sparse_matrix*) malloc(sizeof(omc_sparse_matrix));
  assertStreamPrint(NULL, 0 != A, "Could not allocate data for sparse matrix.");
  /*Uses the CSR-Format*/
  if(ROW_WISE==orientation){
    A->orientation = ROW_WISE;
    A->ptr = (int*) calloc((size_rows+1),sizeof(int));
  }
  /*Uses the CSC-Format*/
  else{
    A->orientation = COLUMN_WISE;
    A->ptr = (int*) calloc((size_cols+1),sizeof(int));
  }

  A->index = (int*) calloc(nnz,sizeof(int));
  A->data= (double*) calloc(nnz,sizeof(double));
  A->size_rows=size_rows;
  A->size_cols=size_cols;
  A->nnz=nnz;

  return(A);
}

/**
 * Deallocates memory for omc_sparse_matrix.
 *
 * \param [ref]    omc_sparse_matrix           Structure.
 */
void
free_sparse_matrix(omc_sparse_matrix* A)
{
  free(A->ptr);
  free(A->index);
  free(A->data);
}

/**
 * Set all Elements in an omc_sparse_matrix to Zero.
 *
 * \param [ref]     omc_sparse_matrix           Structure.
 * \param [out]     omc_sparse_matrix           Structure.
 */
omc_sparse_matrix*
set_zero_sparse_matrix(omc_sparse_matrix* A)
{
  memset(A->index,0,(A->nnz)*sizeof(int));
  memset(A->data,0,(A->nnz)*sizeof(double));

  if(COLUMN_WISE==A->orientation){
    memset(A->ptr,0,(A->size_cols+1)*sizeof(int));
  }
  else{
    memset(A->ptr,0,(A->size_rows+1)*sizeof(int));
  }
  return(A);
}

/**
 * Copies an omc_sparse_matrix.
 *
 * \param [ref]     omc_sparse_matrix           Original Matrix.
 * \param [out]     omc_sparse_matrix           Copied Matrix.
 */
omc_sparse_matrix*
copy_sparse_matrix(omc_sparse_matrix* A)
{
  omc_sparse_matrix* B = allocate_sparse_matrix(A->size_rows, A->size_cols, A->nnz, A->orientation);
  if(COLUMN_WISE == A->orientation){
    memcpy(A->ptr, B->ptr, sizeof(int)*(A->size_cols));
  }
  else{
    memcpy(A->ptr, B->ptr, sizeof(int)*(A->size_rows));
  }
  memcpy(A->index, B->index, sizeof(int)*(A->nnz));
  memcpy(A->data, B->data, sizeof(double)*(A->nnz));
  return(B);
}

/**
 * Sets the (i,j) Element in an omc_sparse_matrix.
 ***CAREFULL WITH ROW_WISE -> ORDER COMES FROM JACOBIAN****
 * \param [ref]     omc_sparse_matrix    Structure.
 * \param [in]      row                  Index Row.
 * \param [in]      col                  Index Column.
 * \param [in]      nth                  Position in Array.
 * \param [in]      value                Value that is set.
 * \param [ref]     omc_sparse_matrix    Structure.
 */
void
set_sparse_matrix_element(omc_sparse_matrix* A, int row, int col, int nth, double value)
{
  if (ROW_WISE == A->orientation){
    if (col>0){
      if(0 == A->ptr[col]){
        A->ptr[col] = nth;
      }
    }
    A->index[nth] = row;
  }
  else{
    if (row>0){
      if (0 == A->ptr[row]){
        A->ptr[row]=nth;
      }
    }
    A->index[nth] = col;
  }
  A->data[nth] = value;
}

/**
 * Gets the (i,j) Element in the omc_sparse_matrix.
 * \param [ref]     omc_sparse_matrix    Structure.
 * \param [in]      row                  Index Row.
 * \param [in]      col                  Index Column.
 * \return          double               Element A(i,j).
 */
double
get_sparse_matrix_element(omc_sparse_matrix* A, int row, int col)
{
  int i, j, c;
  if (COLUMN_WISE == A->orientation){
    int nnz_col;
    c = 0;
    nnz_col = A->ptr[col+1]-A->ptr[col];
    if (0 != nnz_col){
      for (i = A->ptr[col]; i < A->ptr[col+1]; i++){
         if (row == A->index[i]){
           if (1 == nnz_col){
             return A->data[A->ptr[col]];
           } else {
               j = A->ptr[col];
               return A->data[j + c];
             }
         c++;
         }
      }
    }
    return 0.0;
  } else{
    int nnz_row;
    c = 0;
    nnz_row = A->ptr[row+1]-A->ptr[row];
    if (0 != nnz_row){
      for (i = A->ptr[row]; i < A->ptr[row+1]; i++){
        if (col == A->index[i]){
          if (1 == nnz_row){
            return A->data[A->ptr[row]];
          } else {
              j = A->ptr[row];
              return A->data[j + c];
            }
        c++;
        }
     }
   }
    return 0.0;
  }
}

/**
 * Scales the omc_sparse_matrix with a constant Scaling Factor.
 *
 * \param [ref]     omc_sparse_matrix    Structure.
 * \param [in]      scalar               Scaling-Factor.
 * \return          omc_sparse_matrix    Structure.
 */
omc_sparse_matrix*
scale_sparse_matrix(omc_sparse_matrix* A, double scalar)
{
  int i;
  for (i= 0; i<A->nnz; i++){
    A->data[i] = scalar*(A->data[i]);
  }
  return(A);
}

/**
 * Print the omc_sparse_matrix. Just for square matrices!!!
 *
 * \param [ref]     omc_sparse_matrix     Structure.
 * \param [in]      const int             Log Level.
 */

void
print_sparse_matrix(omc_sparse_matrix* A, const int logLevel)
{
  if (ACTIVE_STREAM(logLevel)){
    int i,j,k,l,s;
    int n = A->size_cols;
    int m = A->size_rows;
    s = (n > m)?n:m;

    if (COLUMN_WISE == A->orientation){
      char **buffer = (char**)malloc(sizeof(char*)*s);
      for (l=0; l<s; l++){
        buffer[l] = (char*)malloc(sizeof(char)*s*20);
        buffer[l][0] = 0;
      }
      k = 0;
      for (i = 0; i < n; i++){
        for (j = 0; j < m; j++){
          if ((k < A->ptr[i + 1]) && (A->index[k] == j)){
            sprintf(buffer[j], "%s %5g ", buffer[j], A->data[k]);
            k++;
          }
          else{
            sprintf(buffer[j], "%s %5g ", buffer[j], 0.0);
          }
        }
      }
      for (l = 0; l < s; l++){
        infoStreamPrint(logLevel, 1, "%s", buffer[l]);
        free(buffer[l]);
      }
      free(buffer);
    }
    else{
      char *buffer = (char*)malloc(sizeof(char)*s*20);
      k = 0;
      for (i = 0; i < m; i++){
        buffer[0] = 0;
        for (j = 0; j < n; j++){
          if ((k < A->ptr[i + 1]) && (A->index[k] == j)){
            sprintf(buffer, "%s %5.2g ", buffer, A->data[k]);
            k++;
          }
          else{
            sprintf(buffer, "%s %5.2g ", buffer, 0.0);
          }
        }
        infoStreamPrint(logLevel, 1, "%s", buffer);
      }
      free(buffer);
    }
  }
}
