#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1024
double a[N*N], b[N*N], c[N*N];

void initMatrix( double *mat, int row, int col)
{
  int i, j, idx;

  for ( i = 0; i < row; i++) {
    for ( j = 0; j < col; j++) {
      idx = i * N + j;
      mat[idx] = (double)rand() / RAND_MAX;
    }
  }
}

void printMatrix( double *mat, int row, int col)
{
  int i, j, idx;

  printf( "%d %d\n", row, col);
  for ( i = 0; i < row; i++) {
    for ( j = 0; j < col; j++) {
      idx = i * N + j;
      printf( "%f ", mat[idx]);
    }
    putchar( '\n');
  }
}

void multiplyMatrix(
  int m, int n, int k,
  double *a, double *b, double *c)
{
  int i1, i2, i3, idxA, idxB, idxC;

#pragma omp parallel for
  for ( i1 = 0; i1 < m; i1++) {
    for ( i2 = 0; i2 < n; i2++) {
      idxC = i1 * n + i2;
      c[idxC] = 0.0;
      
      for ( i3 = 0; i3 < k; i3++) {
	idxA = i1 * k + i3;
	idxB = i3 * n + i2;
	c[idxC] += a[idxA] * b[idxB];
      }
    }
  }
}

int main(void) {
  int i, j, k, idx;
  
  initMatrix( a, N, N);
  initMatrix( b, N, N);
  initMatrix( c, N, N);

  printMatrix( a, N, N);
  printMatrix( b, N, N);

  multiplyMatrix( N, N, N, a, b, c);
  
  printMatrix( c, N, N);
}
