// for helping to handle Matrix
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <acml.h>
#include "matrix.h"
#include "myDebug.h"

Matrix *matrixNew( int row, int col, int flag)
{
  Matrix *m;

  if ( IS_FAILED( ( m = malloc( sizeof( Matrix))) != NULL)) {
    return NULL;
  }
  if ( IS_FAILED( ( m->elm = calloc( sizeof( double), row * col)) != NULL)) {
    return NULL;
  }
  m->row = row;
  m->col = col;

  if ( flag) {
    for ( int i = 0; i < row; i++) {
      for ( int j = 0; j < col; j++) {
        int idx = i * col + j;
        m->elm[ idx] = (double)rand() / RAND_MAX;
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

Matrix *matrixReadFile( FILE *fp)
{
  Matrix *m;
  int row, col;
  
  fscanf( fp, "%d%d", &row, &col);
  if ( IS_FAILED( ( m = matrixNew( row, col, 0)) != NULL)) {
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

        c->elm[idxC] += a->elm[idxA] * b->elm[idxB];
      }
    }
  }
}

int matrixTest()
{
  Matrix *a, *b, *c;
  int am, an, bm, bn, cm, cn;
  clock_t cs, ce;
  double t;
  
  for ( int i = 128; i < 10000; i += 128) {
    am = i; an = i;
    bm = an; bn = i;
    cm = am; cn = bn;
  
    a = matrixNew( am, an, 1);
    b = matrixNew( bm, bn, 1);
    c = matrixNew( cm, cn, 0);

    cs = clock();
    dgemm( 'N', 'N', cm, cn, an, 1.0, a->elm, an, b->elm, bn, 0.0, c->elm, cn);
    ce = clock();
    t = (ce - cs) / (double)CLOCKS_PER_SEC;
    
    printf( "size %d, %f s %e GFlops\n", i, t, (double)i * i * i * 2.0 / t / 1000000000.0);

    matrixDelete(a);
    matrixDelete(b);
    matrixDelete(c);
  }
}

int main()
{
  matrixTest();
}
