#include <sys/time.h>
#include <cblas.h>
#include "kadbg.h"
#include "matrix.h"
#include "kalas.h"

#define MAX_BUF_SIZE 128*1024*1024
#define NUM_OF_STEPS 1024


void test_sgemm( KalasState *state)
{
	int rowA, colA, rowB, colB, rowC, colC;
	Matrix *matA = NULL, *matB = NULL, *matC1 = NULL, *matC2 = NULL;
	float alpha = 1.0, beta = 0.0, e;
  struct timeval t;
  double ts, te;

	for ( rowA = NUM_OF_STEPS; rowA <= MAX_BUF_SIZE / NUM_OF_STEPS
					/ sizeof(float);
				rowA += NUM_OF_STEPS) {
		for ( colA = NUM_OF_STEPS; rowA * colA <= MAX_BUF_SIZE / sizeof(float);
					colA += NUM_OF_STEPS) {

			rowB = colA;
			rowC = rowA;
			for ( colB = NUM_OF_STEPS; rowB * colB <= MAX_BUF_SIZE / sizeof(float);
						colB += NUM_OF_STEPS) {
				colC = colB;

				if ( rowC * colC > MAX_BUF_SIZE / sizeof(float)) break;

				if ( IS_FAILED( ( matA = matrixNew( rowA, colA, colA,
																						sizeof(float), true)) != NULL))
					break;
				if ( IS_FAILED( ( matB = matrixNew( rowB, colB, colB,
																						sizeof(float), true)) != NULL))
					break;
				if ( IS_FAILED( ( matC1 = matrixNew( rowC, colC, colC,
																						 sizeof(float), true)) != NULL))
					break;
				if ( IS_FAILED( ( matC2 = matrixNew( rowC, colC, colC,
																						 sizeof(float), true)) != NULL))
					break;

				printf( "(%d,%d)(%d,%d)=(%d,%d) ",
								rowA, colA, rowB, colB, rowC, colC);
				
				gettimeofday( &t, NULL);
				ts = t.tv_sec + (double)(t.tv_usec) / 1000000.0;

				cblas_sgemm( CblasRowMajor, CblasNoTrans, CblasNoTrans,
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
				
				kalasSgemm( state, clblasRowMajor, clblasNoTrans, clblasNoTrans,
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
}

void test_dgemm( KalasState *state)
{
	int rowA, colA, rowB, colB, rowC, colC;
	Matrix *matA = NULL, *matB = NULL, *matC1 = NULL, *matC2 = NULL;
	double alpha = 1.0, beta = 0.0, e;
  struct timeval t;
  double ts, te;

	for ( rowA = NUM_OF_STEPS; rowA <= MAX_BUF_SIZE / NUM_OF_STEPS
					/ sizeof(double);
				rowA += NUM_OF_STEPS) {
		for ( colA = NUM_OF_STEPS; rowA * colA <= MAX_BUF_SIZE / sizeof(double);
					colA += NUM_OF_STEPS) {

			rowB = colA;
			rowC = rowA;
			for ( colB = NUM_OF_STEPS; rowB * colB <= MAX_BUF_SIZE / sizeof(double);
						colB += NUM_OF_STEPS) {
				colC = colB;

				if ( rowC * colC > MAX_BUF_SIZE / sizeof(double)) break;

				if ( IS_FAILED( ( matA = matrixNew( rowA, colA, colA,
																						sizeof(double), true)) != NULL))
					break;
				if ( IS_FAILED( ( matB = matrixNew( rowB, colB, colB,
																						sizeof(double), true)) != NULL))
					break;
				if ( IS_FAILED( ( matC1 = matrixNew( rowC, colC, colC,
																						 sizeof(double), true)) != NULL))
					break;
				if ( IS_FAILED( ( matC2 = matrixNew( rowC, colC, colC,
																						 sizeof(double), true)) != NULL))
					break;

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
}

int main( void)
{
	KalasState *state;

	state = kalasStateNew();
	
	test_sgemm( state);
	test_dgemm( state);

	kalasStateDelete( state);
	return 0;
}
