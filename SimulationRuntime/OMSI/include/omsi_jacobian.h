#ifndef _OMSI_JACOBIAN_H
#define _OMSI_JACOBIAN_H

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

set_omsi_matrix(int row, int col, double* val, void* data)
{
  omsi_matrix* A = (omsi_matrix*) data;
  A[row+col*A->rows] = val;
}

set_omsi_triplet_matrix(int row, int col, double* val, void* data)
{
  omsi_triplet_matrix* A = (omsi_triplet_matrix*) data;

  A[row+col*A->rows] = val;
}



#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif
