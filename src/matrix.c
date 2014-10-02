// for helping to handle Matrix
#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "myDebug.h"

Matrix *matrixNew( int row, int col)
{
  Matrix *m;

  if ( IS_FAILED( ( m = malloc( sizeof( Matrix))) != NULL)) {
    return NULL;
  }
  if ( IS_FAILED( ( m->elm = malloc( sizeof( double) * row * col)) != NULL)) {
    return NULL;
  }
  m->row = row;
  m->col = col;
  return m;
}

void matrixDelete( Matrix *m)
{
  free( m->elm);
  free( m);
}

Matrix *matrixReadFile( FILE *fp)
{
  Matrix *m;
  int row, col;
  
  fscanf( fp, "%d%d", &row, &col);
  if ( IS_FAILED( ( m = matrixNew( row, col)) != NULL)) {
    return NULL;
  }

  for ( int i = 0; i < row * col; i++) fscanf( fp, "%lf", &(m->elm[i]));
  m->row = row;
  m->col = col;
  
  return m;
}

void matrixPrint( Matrix *a)
{
  printf( "%d %d\n", a->row, a->col);
  for ( int i = 0; i < a->row; i++) {
    for ( int j = 0; j < a->col; j++) {
      printf( "%e ", a->elm[ i * a->col + j]);
    }
    printf( "\n");
  }
}

double matrixCalcDiff( Matrix *a, Matrix *b)
{
  double err = 0.0;
  
  if ( a->col == 0 || b->col == 0 || a->col != b->col) return FP_NAN;
  if ( a->row == 0 || b->row == 0 || a->row != b->row) return FP_NAN;
  
  for ( int i = 0; i < a->row * a->col; i++) {
    err += ( a->elm[i] - b->elm[i]) * ( a->elm[i] - b->elm[i]);
  }
  
  return sqrt( err / (double)(a->col * a->row));
}

void matrixMultiply( Matrix *a, Matrix *b, Matrix *c)
{
  for ( int i = 0; i < a->row; i++) {
    for ( int j = 0; j < b->col; j++) {
      int idxC = c->col * i + j;
      c->elm[idxC] = 0;
      
      for ( int k = 0; k < a->col; k++) {
        int idxA, idxB;
        idxA = a->col * i + k;
        idxB = b->col * k + j;

        c->elm[idxC] += a[idxA] * b[idxB];
      }
    }
  }
}

bool matrixTest()
{
  
}
