#ifndef KALAS_H__
#define KALAS_H__
#include <stdio.h>
#include "kalasState.h"

#define CHKCLERR(expr) {if ( (expr) != CL_SUCCESS) { \
    fprintf( stderr, "%s: %d %s: `%s' failed.",              \
             __FILE__, __LINE__, __func__, __STRING(expr));  \
    goto FUNCEXIT;}}

typedef enum { KALAS_FLOAT = 4, KALAS_DOUBLE = 8} typeKind;

int kalasSgemm( KalasState *state, char ta, char tb, int M, int N, int K, float alpha, float *A, int lda, float *B, int ldb, float beta, float *C, int ldc);

int kalasDgemm( KalasState *state, char ta, char tb, int M, int N, int K, double alpha, double *A, int lda, double *B, int ldb, double beta, double *C, int ldc);

#endif // KALAS_H__
