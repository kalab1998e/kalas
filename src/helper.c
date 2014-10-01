// for helping to handle Matrix
#include <stdio.h>
#include <math.h>

typedef struct {
  double *elm;
  int row, col;
} Matrix;

Matrix *createMatrix( int row, int col)
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

bool deleteMatrix( Matrix *m)
{
  delete m->elm;
  delete m;
}

Matrix *readMatrix( FILE *fp)
{
  Matrix *m;
  int row, col;
  
  fscanf( fp, "%d%d", &row, &col);
  if ( IS_FAILED( ( m = createMatrix( row, col)) != NULL)) {
    return NULL;
  }

  for ( int i = 0; i < row * col; i++) fscanf( fp, "%lf", &(m->elm[i]));
  m->row = row;
  m->col = col;
  
  return m;
}

void writeMatrix( Matrix *a)
{
  for ( int i = 0; i < a->row; i++) {
    for ( int j = 0; j < a->col; j++) {
      printf( "%e ", a->elm[ i * a->col + j]);
    }
    printf( "\n");
  }
}

double calcDiff( Matrix *a, Matrix *b)
{
  double err = 0.0;
  
  if ( a->col == 0 || b->col == 0 || a->col != b->col) return FP_NAN;
  if ( a->row == 0 || b->row == 0 || a->row != b->row) return FP_NAN;
  
  for ( int i = 0; i < a->row * a->col; i++) {
    err += ( a->elm[i] - b->elm[i]) * ( a->elm[i] - b->elm[i]);
  }
  
  return sqrt( err / (double)(a->col * a->row));
}

bool test_helper()
{
}
