#ifndef KALAS_H__
#define KALAS_H__
#include <stdio.h>
#include "kalasState.h"

#define CHKCLERR(expr) {if ( (expr) != CL_SUCCESS) { \
    fprintf( stderr, "%s: %d %s: `%s' failed.",              \
             __FILE__, __LINE__, __func__, __STRING(expr));  \
    goto FUNCEXIT;}}

int kalasSgemm(
	KalasState *state, const clblasOrder order,
	const clblasTranspose transA, const clblasTranspose transB,
	const size_t M, const size_t N, const size_t K,
	const float alpha, const float *A, const int lda,
	const float *B, const int ldb, const float beta, float *C, const int ldc);

int kalasDgemm(
	KalasState *state, const clblasOrder order,
	const clblasTranspose transA, const clblasTranspose transB,
	const size_t M, const size_t N, const size_t K,
	const double alpha, const double *A, const int lda,
	const double *B, const int ldb, const double beta, double *C, const int ldc);

#endif // KALAS_H__
