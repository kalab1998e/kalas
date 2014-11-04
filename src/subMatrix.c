#include <stdlib.h>
#include <stdbool.h>
#include "subMatrix.h"
#include "matrix.h"
#include "kadbg.h"

SubMatrix *subMatrixNew( Matrix *mat, int row_, int col_)
{
	SubMatrix *smat;
	int row, col;
		;
	if ( IS_FAILED( ( smat = (SubMatrix*)malloc( sizeof(Matrix*) * row_ * col_))
									!= NULL)) {
		return NULL;
	}

	smat->subRow = row_;
	smat->subCol = col_;
	smat->noRow = mat->row / row_ + (mat->row % row_ == 0) ? 0 : 1;
	smat->noCol = mat->col / col_ + (mat->col % col_ == 0) ? 0 : 1;

	for ( int iblk = 0; iblk < smat->noRow; iblk++) {
		for ( int jblk = 0; jblk < smat->noCol; jblk++) {
			for ( int ielm = 0; ielm < smat->row; ielm++) {
				for ( int jelm = 0; jelm < smat->col; jelm++) {
				}
			}
		}
	}
}

bool subMatrixDelete( SubMatrix *smat)
Matrix *subMatrixGetBlock( SubMatrix *smat, int m, int n)
bool subMatrixSetBlock( SubMatrix *smat, int m, int n, Matrix *mat)
