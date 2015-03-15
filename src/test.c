#include <sys/time.h>
#include <cblas.h>
#include "kadbg.h"
#include "matrix.h"
#include "kalas.h"

#define MAX_BUF_SIZE 128*1024*1024
#define NUM_OF_STEPS 1024

int main(void)
{
  KalasState *state;
	int rowA, colA, rowB, colB, rowC, colC;
	Matrix *matA = NULL, *matB = NULL, *matC1 = NULL, *matC2 = NULL;
	double alpha = 1.0, beta = 0.0, e;
  struct timeval t;
  double ts, te;

  state = kalasStateNew();
	
	for ( rowA = NUM_OF_STEPS; rowA <= MAX_BUF_SIZE / sizeof(double);
				rowA += NUM_OF_STEPS) {
		for ( colA = NUM_OF_STEPS; colA <= MAX_BUF_SIZE / sizeof(double);
					colA += NUM_OF_STEPS) {
			if ( rowA * colA * sizeof(double) > MAX_BUF_SIZE) break;

			rowB = colA;
			for ( colB = NUM_OF_STEPS; colB <= MAX_BUF_SIZE / sizeof(double);
						colB += NUM_OF_STEPS) {
				rowC = rowA;
				colC = colB;

				if ( rowB * colB * sizeof(double) > MAX_BUF_SIZE) break;
				if ( rowC * colC * sizeof(double) > MAX_BUF_SIZE) break;

				if ( IS_FAILED( ( matA = matrixNew( rowA, colA, colA,
																						sizeof(double), true)) != NULL))
					return 1;
				if ( IS_FAILED( ( matB = matrixNew( rowB, colB, colB,
																						sizeof(double), true)) != NULL))
					return 1;
				if ( IS_FAILED( ( matC1 = matrixNew( rowC, colC, colC,
																						 sizeof(double), true)) != NULL))
					return 1;
				if ( IS_FAILED( ( matC2 = matrixNew( rowC, colC, colC,
																						 sizeof(double), true)) != NULL))
					return 1;

				printf( "(%d,%d)(%d,%d)=(%d,%d) ",
								rowA, colA, rowB, colB, rowC, colC);
				
				gettimeofday( &t, NULL);
				ts = t.tv_sec + (double)(t.tv_usec) / 1000000.0;

				cblas_dgemm( CblasRowMajor, CblasNoTrans, CblasNoTrans,
										 matC1->row, matC1->col, matA->col,
										 alpha, matA->elm, matA->col,
										 matB->elm, matB->col, beta,
										 matC1->elm, matC1->col);
				
				gettimeofday( &t, NULL);
				te = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
				printf( "%f s %f GFlops ", (te - ts),
								(double)rowA * (double)colA * (double)colB * 2.0
								/ ( te - ts) / 1000000000.0);

				gettimeofday( &t, NULL);
				ts = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
				
				kalasDgemm( state, clblasRowMajor, clblasNoTrans, clblasNoTrans,
										matC2->row, matC2->col, matA->col,
										alpha, matA->elm, matA->ld,
										matB->elm, matB->ld, beta,
										matC2->elm, matC2->ld);
				
				gettimeofday( &t, NULL);
				te = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
				printf( "%f s %f GFlops ", (te - ts),
								(double)rowA * (double)colA * (double)colB * 2.0
								/ ( te - ts) / 1000000000.0);

				printf( "%e err", matrixCalcDiff( matC1, matC2));
				putchar( '\n');
				matrixDelete( matA); matA = NULL;
				matrixDelete( matB); matB = NULL;
				matrixDelete( matC1); matC1 = NULL;
				matrixDelete( matC2); matC2 = NULL;
			}
		}
  }

	kalasStateDelete( state);
	return 0;
}
