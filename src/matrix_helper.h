#ifndef MATRIX_H__
#define MATRIX_H__

typedef struct {
  double *elm;
  int row, col;
} Matrix;

Matrix *matrixNew( int row, int col);
void matrixDelete( Matrix *m);
Matrix *matrixReadFile( FILE *fp):
void matrixPrint( Matrix *a);
double matrixCalcDiff( Matrix *a, Matrix *b);
bool matrixTest();

#endif // MATRIX_H__
