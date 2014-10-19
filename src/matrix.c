// for helping to handle Matrix
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "matrix.h"
#include "kadbg.h"

Matrix *matrixNew( int row, int col, typeKind type, int flag)
{
  Matrix *m;
  
  if ( IS_FAILED( ( m = malloc( sizeof( Matrix))) != NULL)) {
    return NULL;
  }
  if ( IS_FAILED( ( m->elm = calloc( type, row * col)) != NULL)) {
    return NULL;
  }
  m->row = row;
  m->col = col;

  if ( flag) {
    for ( int i = 0; i < row; i++) {
      for ( int j = 0; j < col; j++) {
        int idx = i * col + j;
        switch ( type) {
        case KALAS_FLOAT:
          ((float*)(m->elm))[ idx] = (float)rand() / RAND_MAX;
          break;
        case KALAS_DOUBLE:
          ((double*)(m->elm))[ idx] = (double)rand() / RAND_MAX;
          break;
        }
      }
    }
  }
  return m;
}

void matrixDelete( Matrix *m)
{
  free( m->elm);
  free( m);
}

Matrix *matrixReadFile( FILE *fp, typeKind type)
{
  Matrix *m;
  int row, col;
  float *f;
  double *d;
  
  fscanf( fp, "%d%d", &row, &col);
  if ( IS_FAILED( ( m = matrixNew( row, col, type, 0)) != NULL)) {
    return NULL;
  }
  
  for ( int i = 0; i < row * col; i++) {
    switch ( type) {
    case KALAS_FLOAT:
      fscanf( fp, "%f", &(((float*)(m->elm))[i]));
      break;
    case KALAS_DOUBLE:
      fscanf( fp, "%lf", &(((double*)(m->elm))[i]));
      break;
    }
  }
  m->row = row;
  m->col = col;
  m->type = type;
  return m;
}

void matrixPrint( Matrix *a)
{
  printf( "%d %d\n", a->row, a->col);
  for ( int i = 0; i < a->row; i++) {
    for ( int j = 0; j < a->col; j++) {
      switch ( a->type) {
      case KALAS_FLOAT:
        printf( "%e ", ((float*)(a->elm))[ i * a->col + j]);
        break;
      case KALAS_DOUBLE:
        printf( "%e ", ((double*)(a->elm))[ i * a->col + j]);
        break;
      }
    }
    printf( "\n");
  }
}

double matrixCalcDiff( Matrix *a, Matrix *b)
{
  double err = 0.0;

  if ( a->type != b->type) return (double)INFINITY;
  
  if ( a->col == 0 || b->col == 0 || a->col != b->col) return (double)INFINITY;
  if ( a->row == 0 || b->row == 0 || a->row != b->row) return (double)INFINITY;
  
  for ( int i = 0; i < a->row * a->col; i++) {
    switch ( a->type) {
    case KALAS_FLOAT:
      err += ( ((float*)(a->elm))[i] - ((float*)(b->elm))[i])
        * ( ((float*)(a->elm))[i] - ((float*)(b->elm))[i]);
      break;
    case KALAS_DOUBLE:
      err += ( ((double*)(a->elm))[i] - ((double*)(b->elm))[i])
        * ( ((double*)(a->elm))[i] - ((double*)(b->elm))[i]);
      break;
    }
  }
  
  return sqrt( err / (double)(a->col * a->row));
}

bool matrixMultiply( Matrix *a, Matrix *b, Matrix *c)
{
  typeKind type = a->type;
  
  for ( int i = 0; i < a->row; i++) {
    for ( int j = 0; j < b->col; j++) {
      int idxC = c->col * i + j;
      switch ( type) {
      case KALAS_FLOAT:
        ((float*)c->elm)[idxC] = 0.0;
        break;
      case KALAS_DOUBLE:
        ((double*)c->elm)[idxC] = 0.0;
        break;
      }
      
      for ( int k = 0; k < a->col; k++) {
        int idxA, idxB;
        idxA = a->col * i + k;
        idxB = b->col * k + j;

        switch (type) {
        case KALAS_FLOAT:
          ((float*)c->elm)[idxC] += ((float*)a->elm)[idxA]
            * ((float*)b->elm)[idxB];
          break;
        case KALAS_DOUBLE:
          ((double*)c->elm)[idxC] += ((double*)a->elm)[idxA]
            * ((double*)b->elm)[idxB];
          break;
        }
      }
    }
  }
}
/*
int matrixTest()
{
  Matrix *a, *b, *c;
  int am, an, bm, bn, cm, cn;
  clock_t cs, ce;
  float t;
  
  for ( int i = 128; i < 1025; i += 128) {
    am = i; an = i;
    bm = an; bn = i;
    cm = am; cn = bn;
  
    a = matrixNew( am, an, KALAS_FLOAT, 1);
    b = matrixNew( bm, bn, KALAS_FLOAT, 1);
    c = matrixNew( cm, cn, KALAS_FLOAT, 0);

    cs = clock();
    sgemm( 'N', 'N', cm, cn, an, 1.0, (float*)(a->elm), an, (float*)(b->elm), bn, 0.0, (float*)(c->elm), cn);
    ce = clock();
    t = (ce - cs) / (float)CLOCKS_PER_SEC;
    
    printf( "size %d, %f s %e GFlops\n", i, t, (float)i * i * i * 2.0 / t / 1000000000.0);

    matrixDelete(a);
    matrixDelete(b);
    matrixDelete(c);
  }
}
*/
/*
int main()
{
  matrixTest();
}
*/
