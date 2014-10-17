#include <stdio.h>
#include <stdbool.h>
#include "kalasState.h"
#include "kalas.h"


static const clblasOrder order = clblasRowMajor;

void releaseObjects( cl_mem *buf, int noOfBuf)
{
  for ( int i = 0; i < noOfBuf; i++) {
    if ( buf[i] != NULL) clReleaseMemObject(buf[i]);
    buf[i] = NULL;
  }
  return;
}

clblasTranspose transChgF2C( char t) {
  switch ( t) {
  case 'N':
    return clblasNoTrans;
  case 'T':
    return clblasTrans;
  }
  return clblasNoTrans;
}

int kalasGemm( KalasState *state, typeKind type, char ta, char tb, int M, int N, int K, double alpha, void *A, int lda, void *B, int ldb, double beta, void *C, int ldc) {
  clblastTranspose transA, transB;
  cl_int err = CL_SUCCESS;
  int noOfBuf = 3;
  cl_mem buf[ noOfBuf];
  cl_event event = NULL;
  
  transA = transChgF2C( ta);
  transB = transChgF2C( tb);
  buf[0] = buf[1] = buf[2] = NULL;
  
  /* Prepare OpenCL memory objects and place matrices inside them. */
  CHKCLERR( ( buf[0] = clCreateBuffer(ctx, CL_MEM_READ_ONLY,
                                      M * K * type, NULL, &err), err));
  CHKCLERR( ( buf[1] = clCreateBuffer(ctx, CL_MEM_READ_ONLY,
                                      K * N * type, NULL, &err), err));
  CHKCLERR( ( buf[2] = clCreateBuffer(ctx, CL_MEM_READ_WRITE,
                                      M * N * type, NULL, &err), err));

  CHKCLERR( err = clEnqueueWriteBuffer(queue, buf[0], CL_TRUE, 0,
                                       M * K * type, A, 0, NULL, NULL));
  CHKCLERR( err = clEnqueueWriteBuffer(queue, buf[1], CL_TRUE, 0,
                                       K * N * type, B, 0, NULL, NULL));
  CHKCLERR( err = clEnqueueWriteBuffer(queue, buf[2], CL_TRUE, 0,
                                       M * N * type, C, 0, NULL, NULL));

  /* Call clblas extended function. Perform gemm for the lower right sub-matrices */
  switch ( type) {
  case KALAS_FLOAT:
    CHKCLERR( err = clblasSgemm(order, transA, transB, M, N, K,
                                alpha, buf[0], 0, lda,
                                buf[1], 0, ldb, beta, buf[2], 0, ldc,
                                1, &queue, 0, NULL, &event));
    break;
  case KALAS_DOUBLE:
    CHKCLERR( err = clblasDgemm(order, transA, transB, M, N, K,
                                alpha, buf[0], 0, lda, 
                                buf[1], 0, ldb, beta, buf[2], 0, ldc,
                                1, &queue, 0, NULL, &event));
    break;
  }

  /* Wait for calculations to be finished. */
  CHKCLERR( err = clWaitForEvents(1, &event));
    
  /* Fetch results of calculations from GPU memory. */
  CHKCLERR( err = clEnqueueReadBuffer(queue, buf[2], CL_TRUE, 0,
                                      M * N * type,
                                      C, 0, NULL, NULL));

 FUNCEXIT:
  if ( err != CL_SUCCESS)
    fprintf( stderr, "%s(%d)\n", clErrNo2Str(err), err);
    
  /* Release OpenCL memory objects. */
  releaseObjects( buf, noOfBuf);
  return err;
}

int kalasSgemm( KalasState *state, char ta, char tb, int M, int N, int K, float alpha, float *A, int lda, float *B, int ldb, float beta, float *C, int ldc)
{
  return kalasGemm( state, KALAS_FLOAT, ta, tb, M, N, K,
                    (double)alpha, (void *)A, lda,
                    (void *)B, ldb, (double)beta, (void *)C, ldc);
}
  
int kalasDgemm( KalasState *state, char ta, char tb, int M, int N, int K, double alpha, double *A, int lda, double *B, int ldb, double beta, double *C, int ldc)
{
  return kalasGemm( state, KALAS_DOUBLE, ta, tb, M, N, K,
                    alpha, (void *)A, lda,
                    (void *)B, ldb, beta, (void *)C, ldc);
}
  
 
  

