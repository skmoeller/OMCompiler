/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2014, Open Source Modelica Consortium (OSMC),
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

/*! \file nonlinear_solver.c
 */
#include "../../../include/solver/TotalPivot.h"




void getIndicesOfPivotElementLS(int *n, int *m, int *l, double* A, int *indRow, int *indCol, int *pRow, int *pCol, double *absMax)
{
  int i, j;

  *absMax = fabs(A[indRow[*l] + indCol[*l]* *n]);
  *pCol = *l;
  *pRow = *l;
  for (i = *l; i < *n; i++) {
   for (j = *l; j < *m; j++) {
      if (fabs(A[indRow[i] + indCol[j]* *n]) > *absMax) {
        *absMax = fabs(A[indRow[i] + indCol[j]* *n]);
        *pCol = j;
        *pRow = i;
      }
    }
  }
}

/*! \ linear solver for A*x = b based on a total pivot search
 *  \ input matrix Ab: first n columns are matrix A
 *                     last column is -b
 *    output x: solution dim n+1, last column is 1 for solvable systems!
 *           rank: rank of matrix Ab
 *           det: determinant of matrix A
 *
 *  \author bbachmann
 */

int solveSystemWithTotalPivotSearchLS(int n, double* x, double* A, int* indRow, int* indCol, int *rank)
{
   int i, k, j, l, m=n+1, nrsh=1, singular=0;
   int pCol, pRow;
   double hValue;
   double hInt;
   double absMax;
   int r,s;
   int permutation = 1;

   /* assume full rank of matrix [n x (n+1)] */
   *rank = n;

   for (i=0; i<n; i++) {
      indRow[i] = i;
   }
   for (i=0; i<m; i++) {
      indCol[i] = i;
   }

   for (i = 0; i < n; i++) {
    getIndicesOfPivotElementLS(&n, &n, &i, A, indRow, indCol, &pRow, &pCol, &absMax);
    /* this criteria should be evaluated and may be improved in future */
    /*
    if (absMax<DBL_EPSILON) {
      *rank = i;
      warningStreamPrint(LOG_LS, 0, "Matrix singular!");
      debugIntLS(LOG_LS,"rank = ", *rank);
      break;
    }
    */
    /* swap row indices */
    if (pRow!=i) {
      hInt = indRow[i];
      indRow[i] = indRow[pRow];
      indRow[pRow] = hInt;
    }
    /* swap column indices */
    if (pCol!=i) {
      hInt = indCol[i];
      indCol[i] = indCol[pCol];
      indCol[pCol] = hInt;
    }

    /* Gauss elimination of row indRow[i] */
    for (k=i+1; k<n; k++) {
      hValue = -A[indRow[k] + indCol[i]*n]/A[indRow[i] + indCol[i]*n];
      for (j=i+1; j<m; j++) {
        A[indRow[k] + indCol[j]*n] = A[indRow[k] + indCol[j]*n] + hValue*A[indRow[i] + indCol[j]*n];
      }
      A[indRow[k] + indCol[i]*n] = 0;
    }
  }

  //debugMatrixDoubleLS(LOG_LS_V,"LGS: matrix Ab manipulated",A, n, n+1);
  /* solve even singular matrix !!! */
  for (i=n-1;i>=0; i--) {
    if (i>=*rank) {
      /* this criteria should be evaluated and may be improved in future */
      if (fabs(A[indRow[i] + n*n])>1e-12) {
    	  //warningStreamPrint(LOG_LS, 0, "under-determined linear system not solvable!");
        return -1;
      } else {
        x[indCol[i]] = 0.0;
      }
    } else {
      x[indCol[i]] = -A[indRow[i] + n*n];
      for (j=n-1; j>i; j--) {
        x[indCol[i]] = x[indCol[i]] - A[indRow[i] + indCol[j]*n]*x[indCol[j]];
      }
      x[indCol[i]]=x[indCol[i]]/A[indRow[i] + indCol[i]*n];
    }
  }
  x[n]=1.0;
  //debugVectorDoubleLS(LOG_LS_V,"LGS: solution vector x",x, n+1);

  return 0;
}


