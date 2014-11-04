#ifndef SUB_MATRIX_H__
#define SUB_MATRIX_H__
#include <stdbool.h>
#include "matrix.h"

typedef struct {
	int row, col, noRow, noCol;
	Matrix **block;
} SubMatrix;

SubMatrix *subMatrixNew( Matrix*mat, int row, int col);
bool subMatrixDelete( SubMatrix *smat);
Matrix *subMatrixGetBlock( SubMatrix *smat, int m, int n);
bool subMatrixSetBlock( SubMatrix *smat, int m, int n, Matrix *mat);

#endif // SUB_MATRIX_H__

