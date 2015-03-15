#include <stdio.h>
#include <stdbool.h>
#include <clBLAS.h>
#include "kalasState.h"
#include "kalas.h"
#include "clerr.h"
#include "kadbg.h"

int kalasGemm(
  KalasState *state, const int type, 	const clblasOrder order,
	const clblasTranspose transA, const clblasTranspose transB,
	const size_t M, const size_t N, const size_t K,
	const double alpha, const void *A, const int lda,
	const void *B, const int ldb, const double beta, void *C, const int ldc)
{
  cl_int err = CL_SUCCESS;
	int numOfBuf = 3;
  cl_mem buf[ numOfBuf];
  cl_event event = NULL;

  buf[0] = buf[1] = buf[2] = NULL;
  
  /* Prepare OpenCL memory objects and place matrices inside them. */
  CHKCLERR( ( buf[0] = clCreateBuffer(state->context, CL_MEM_READ_ONLY,
                                      M * lda * type, NULL, &err), err));
  CHKCLERR( ( buf[1] = clCreateBuffer(state->context, CL_MEM_READ_ONLY,
                                      K * ldb * type, NULL, &err), err));
  CHKCLERR( ( buf[2] = clCreateBuffer(state->context, CL_MEM_READ_WRITE,
                                      M * ldc * type, NULL, &err), err));

  CHKCLERR( err = clEnqueueWriteBuffer(state->queue[0], buf[0], CL_TRUE, 0,
                                       M * lda * type, A, 0, NULL, NULL));
  CHKCLERR( err = clEnqueueWriteBuffer(state->queue[0], buf[1], CL_TRUE, 0,
                                       K * ldb * type, B, 0, NULL, NULL));
  CHKCLERR( err = clEnqueueWriteBuffer(state->queue[0], buf[2], CL_TRUE, 0,
                                       M * ldc * type, C, 0, NULL, NULL));

  /* Call clblas extended function. Perform gemm for the lower right sub-matrices */
  switch ( type) {
  case sizeof(float):
    CHKCLERR( err = clblasSgemm(order, transA, transB, M, N, K,
                                (float)alpha, buf[0], 0, lda,
                                buf[1], 0, ldb, (float)beta, buf[2], 0, ldc,
                                1, &(state->queue[0]), 0, NULL, &event));
    break;
  case sizeof(double):
    CHKCLERR( err = clblasDgemm(order, transA, transB, M, N, K,
                                alpha, buf[0], 0, lda, 
                                buf[1], 0, ldb, beta, buf[2], 0, ldc,
                                1, &(state->queue[0]), 0, NULL, &event));
    break;
  }

  /* Wait for calculations to be finished. */
  CHKCLERR( err = clWaitForEvents(1, &event));
    
  /* Fetch results of calculations from GPU memory. */
  CHKCLERR( err = clEnqueueReadBuffer(state->queue[0], buf[2], CL_TRUE, 0,
                                      M * N * type,
                                      C, 0, NULL, NULL));

 FUNCEXIT:
  if ( IS_FAILED( err == CL_SUCCESS)) {
    fprintf( stderr, "%s(%d)\n", clErrNo2Str(err), err);
	}
    
  /* Release OpenCL memory objects. */
  for ( int i = 0; i < numOfBuf; i++) {
    if ( buf[i] != NULL) clReleaseMemObject(buf[i]);
    buf[i] = NULL;
  }
  return err;
}

int kalasSgemm(
	KalasState *state, const clblasOrder order,
	const clblasTranspose transA, const clblasTranspose transB,
	const size_t M, const size_t N, const size_t K,
	const float alpha, const float *A, const int lda,
	const float *B, const int ldb, const float beta, float *C, const int ldc)
{
  return kalasGemm( state, sizeof(float), order, transA, transB, M, N, K,
                    (double)alpha, (void *)A, lda,
                    (void *)B, ldb, (double)beta, (void *)C, ldc);
}
  
int kalasDgemm(
	KalasState *state, const clblasOrder order,
	const clblasTranspose transA, const clblasTranspose transB,
	const size_t M, const size_t N, const size_t K,
	const double alpha, const double *A, const int lda,
	const double *B, const int ldb, const double beta, double *C, const int ldc)
{
  return kalasGemm( state, sizeof(double), order, transA, transB, M, N, K,
                    alpha, (void*)A, lda, (void*)B, ldb, beta, (void*)C, ldc);
}
  
 
  

