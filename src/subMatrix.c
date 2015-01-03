#include <stdlib.h>
#include <stdbool.h>
#include "subMatrix.h"
#include "matrix.h"
#include "kadbg.h"

SubMatrix *subMatrixNew( Matrix *mat, int row_, int col_)
{
	SubMatrix *smat;
	int row, col;

	if ( IS_FAILED( ( smat = (SubMatrix*)malloc( sizeof(SubMatrix*)))
									!= NULL)) {
		return NULL;
	}

	smat->subRow = row_;
	smat->subCol = col_;
	smat->numRow = mat->row / row_ + (mat->row % row_ == 0) ? 0 : 1;
	smat->numCol = mat->col / col_ + (mat->col % col_ == 0) ? 0 : 1;
	if ( IS_FAILED( ( smat->matBlock = (Matrinx*)malloc( sizeof( Matrix*)))
									!= NULL)) {
		return NULL;
	}
	
	for ( int iblk = 0; iblk < smat->numRow; iblk++) {
		for ( int jblk = 0; jblk < smat->numCol; jblk++) {
			if ( IS_FAILED( ( (smat->matBlock)[iblk][jblk] =
												matrixNew( smat->subRow, smat->subCol, smat->subCol,
																	 mat->type, false)) != NULL)) {
				return NULL;
			}
			for ( int ielm = 0; ielm < smat->row; ielm++) {
				int iorg = iblk * smat->row + ielm;
				for ( int jelm = 0; jelm < smat->col; jelm++) {
					int jorg = jblk * smat->col + jelm;
					switch mat->type 
					mat->elm[
				}
			}
		}
	}
}

bool subMatrixDelete( SubMatrix *smat)
Matrix *subMatrixGetBlock( SubMatrix *smat, int m, int n)
bool subMatrixSetBlock( SubMatrix *smat, int m, int n, Matrix *mat)
