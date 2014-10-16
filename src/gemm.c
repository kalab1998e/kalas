#include "kalasState.h"
#include "kalas.h"

static const clblasOrder order = clblasRowMajor;

clblasTranspose transChgF2C( char t) {
  switch ( t) {
  case 'N':
    return clblasNoTrans;
  case 'T':
    return clblasTrans;
  }
  return clblasNoTrans;
}

void kalasGemm( kalasState *state, typeKind type, char ta, char tb, int M, int N, int K, double alpha, void *a, int lda, void *b, int ldb, double beta, void *c, int ldc) {
  clblastTranspose transA, transB;
  cl_int err;
  cl_mem bufA, bufB, bufC;
  cl_event event = NULL;
  int ret = 0;
  
  transA = transChgF2C( ta);
  transB = transChgF2C( tb);
  
}
