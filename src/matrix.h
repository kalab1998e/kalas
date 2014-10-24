#ifndef MATRIX_H__
#define MATRIX_H__

#include <stdio.h>
#include <stdbool.h>
#include "kalas.h"

typedef struct {
  void *elm;
  int row, col, ld;
  typeKind type;
} Matrix;

Matrix *matrixNew( int row, int col, int ld, typeKind type, int flag);
void matrixDelete( Matrix *m);
Matrix *matrixReadFile( FILE *fp, typeKind type);
void matrixPrint( Matrix *a);
double matrixCalcDiff( Matrix *a, Matrix *b);
bool matrixMultiply( Matrix *a, Matrix *b, Matrix *c);
int matrixTest();

#endif // MATRIX_H__
