#ifndef MATRIX_H__
#define MATRIX_H__

#include <stdio.h>

typedef struct {
  float *elm;
  int row, col;
} Matrix;

Matrix *matrixNew( int row, int col, int flag);
void matrixDelete( Matrix *m);
Matrix *matrixReadFile( FILE *fp);
void matrixPrint( Matrix *a);
float matrixCalcDiff( Matrix *a, Matrix *b);
void matrixMultiply( Matrix *a, Matrix *b, Matrix *c);
int matrixTest();

#endif // MATRIX_H__
