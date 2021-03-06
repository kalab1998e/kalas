// for helping to handle Matrix
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "matrix.h"
#include "kadbg.h"

Matrix *matrixNew( int row, int col, int ld, int type, bool flag)
{
  Matrix *m;

  if ( col > ld) ld = col;
  if ( IS_FAILED( ( m = malloc( sizeof( Matrix))) != NULL)) {
    return NULL;
  }
  if ( IS_FAILED( ( m->elm = calloc( type, row * ld)) != NULL)) {
    return NULL;
  }
  m->row = row;
  m->col = col;
  m->ld = ld;
  m->type = type;
  
  if ( flag) {
    for ( int i = 0; i < row; i++) {
      for ( int j = 0; j < col; j++) {
        int idx = i * ld + j;
        switch ( type) {
        case sizeof(float):
          ((float*)(m->elm))[ idx] = (float)rand() / RAND_MAX;
          break;
        case sizeof(double):
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

Matrix *matrixReadFile( FILE *fp, int type)
{
  Matrix *m;
  int row, col;
  float *f;
  double *d;
  
  fscanf( fp, "%d%d", &row, &col);
  if ( IS_FAILED( ( m = matrixNew( row, col, col, type, 0)) != NULL)) {
    return NULL;
  }
  
  for ( int i = 0; i < row * col; i++) {
    switch ( type) {
    case sizeof(float):
      fscanf( fp, "%f", &(((float*)(m->elm))[i]));
      break;
    case sizeof(double):
      fscanf( fp, "%lf", &(((double*)(m->elm))[i]));
      break;
    }
  }
  return m;
}

void matrixPrint( Matrix *a)
{
  printf( "%d %d type:%d\n", a->row, a->col, a->type);
  for ( int i = 0; i < a->row; i++) {
    for ( int j = 0; j < a->col; j++) {
      int idx = i * a->ld + j;
      
      switch ( a->type) {
      case sizeof(float):
        printf( "%e ", ((float*)(a->elm))[ idx]);
        break;
      case sizeof(double):
        printf( "%e ", ((double*)(a->elm))[ idx]);
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
  
  for ( int i = 0; i < a->row * a->ld; i++) {
    switch ( a->type) {
    case sizeof(float):
      err += ( ((float*)(a->elm))[i] - ((float*)(b->elm))[i])
        * ( ((float*)(a->elm))[i] - ((float*)(b->elm))[i]);
      break;
    case sizeof(double):
      err += ( ((double*)(a->elm))[i] - ((double*)(b->elm))[i])
        * ( ((double*)(a->elm))[i] - ((double*)(b->elm))[i]);
      break;
    }
  }
  
  return sqrt( err / (double)(a->col * a->row));
}

bool matrixMultiply( Matrix *a, Matrix *b, Matrix *c)
{
  int type = a->type;
  
  for ( int i = 0; i < a->row; i++) {
    for ( int j = 0; j < b->col; j++) {
      int idxC = c->ld * i + j;
      switch ( type) {
      case sizeof(float):
        ((float*)c->elm)[idxC] = 0.0;
        break;
      case sizeof(double):
        ((double*)c->elm)[idxC] = 0.0;
        break;
      }
      
      for ( int k = 0; k < a->col; k++) {
        int idxA, idxB;
        idxA = a->ld * i + k;
        idxB = b->ld * k + j;

        switch (type) {
        case sizeof(float):
          ((float*)c->elm)[idxC] += ((float*)a->elm)[idxA]
            * ((float*)b->elm)[idxB];
          break;
        case sizeof(double):
          ((double*)c->elm)[idxC] += ((double*)a->elm)[idxA]
            * ((double*)b->elm)[idxB];
          break;
        }
      }
    }
  }
}

Matrix *matrixGetSubMatrix( Matrix *a, int rs, int cs, int re, int ce)
{
	Matrix *b;
	int brow, bcol, brow_, bcol_, re_, ce_;

	if ( ce >= a->col) ce_ = a->col - 1;
	if ( re >= a->row) re_ = a->row - 1;

	brow = re - rs + 1;
	brow_ = re_ - rs + 1;
	bcol = ce - cs + 1;
	bcol_ = ce_ - cs + 1;
	if ( IS_FAILED( ( b = matrixNew( brow, bcol, bcol, a->type, 0))
									!= NULL)) return NULL;

	for ( int i = 0; i < brow_; i++) {
		for ( int j = 0; j < bcol_; j++) {
			int idx, bidx;
			idx = ( rs + i) * a->ld + cs + j;
			bidx = i * bcol + j;
			switch ( a->type) {
			case sizeof(float):
				((float*)( b->elm))[ bidx] = ((float*)(a->elm))[ idx];
				break;
			case sizeof(double):
				((double*)( b->elm))[ bidx] = ((double*)(a->elm))[ idx];
				break;
			}
		}
	}

	return b;
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
