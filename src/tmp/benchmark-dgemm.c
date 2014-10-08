#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include <acml.h>
#include <clBLAS.h>

#define N 4096
#define STEP 128

void loadMatrix( double *mat, int row, int col)
{
  int i, j, idx;

  for ( i = 0; i < row; i++) {
    for ( j = 0; j < col; j++) {
      idx = i + j * row;
      scanf( "%lf", &( mat[ idx]));
    }
  }
}

void initMatrix( double *mat, int row, int col)
{
  int i, j, idx;

  for ( i = 0; i < row; i++) {
    for ( j = 0; j < col; j++) {
      idx = i + j * row;
      mat[idx] = (double)rand() / RAND_MAX;
    }
  }
}

double calcErr( double *mat1, double *mat2, int row, int col)
{
  int i, j, idx;
  double err;

  err = 0.0;
  for ( i = 0; i < row; i++) {
    for ( j = 0; j < col; j++) {
      idx = i + j * row;
      err = err + sqrt( ( mat1[ idx] - mat2[ idx])
			* ( mat1[ idx] - mat2[ idx]));
    }
  }

  err = err / ( row * col);
  return err;
}

void gpuDgemm( char ta, char tb, int m, int n, int k,
	       double alpha, double *a, int lda,
	       double *b, int ldb, double beta, double *c, int ldc)
{
  cl_int err;
  cl_platform_id platform = 0;
  cl_device_id device = 0;
  cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, 0, 0 };
  cl_context ctx = 0;
  cl_command_queue queue = 0;
  cl_mem bufA, bufB, bufC;
  cl_event event = NULL;
  clblasTranspose transA, transB;
  int ret = 0;

  switch ( ta) {
  case 'T':
    transA = clblasTrans;
    break;
  case 'C':
    transA = clblasConjTrans;
    break;
  case 'N':
  default:
    transA = clblasNoTrans;
    break;
  }
    
  switch ( tb) {
  case 'T':
    transB = clblasTrans;
    break;
  case 'C':
    transB = clblasConjTrans;
    break;
  case 'N':
  default:
    transB = clblasNoTrans;
    break;
  }

  /* Setup OpenCL environment. */
  err = clGetPlatformIDs(1, &platform, NULL);
  if (err != CL_SUCCESS) {
    printf( "clGetPlatformIDs() failed with %d\n", err );
    return 1;
  }

  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
  if (err != CL_SUCCESS) {
    printf( "clGetDeviceIDs() failed with %d\n", err );
    return 1;
  }

  props[1] = (cl_context_properties)platform;
  ctx = clCreateContext(props, 1, &device, NULL, NULL, &err);
  if (err != CL_SUCCESS) {
    printf( "clCreateContext() failed with %d\n", err );
    return 1;
  }

  queue = clCreateCommandQueue(ctx, device, 0, &err);
  if (err != CL_SUCCESS) {
    printf( "clCreateCommandQueue() failed with %d\n", err );
    clReleaseContext(ctx);
    return 1;
  }

  /* Setup clblas. */
  /*  err = clblasSetup();
  if (err != CL_SUCCESS) {
    printf("clblasSetup() failed with %d\n", err);
    clReleaseCommandQueue(queue);
    clReleaseContext(ctx);
    return 1;
  }
  */

  /* Prepare OpenCL memory objects and place matrices inside them. */
  bufA = clCreateBuffer(ctx, CL_MEM_READ_ONLY, m * k * sizeof(*a),
			NULL, &err);
  bufB = clCreateBuffer(ctx, CL_MEM_READ_ONLY, k * n * sizeof(*b),
			NULL, &err);
  bufC = clCreateBuffer(ctx, CL_MEM_READ_WRITE, m * n * sizeof(*c),
			NULL, &err);

  err = clEnqueueWriteBuffer(queue, bufA, CL_TRUE, 0,
			     m * n * sizeof(*a), a, 0, NULL, NULL);
  err = clEnqueueWriteBuffer(queue, bufB, CL_TRUE, 0,
			     k * n * sizeof(*b), b, 0, NULL, NULL);
  err = clEnqueueWriteBuffer(queue, bufC, CL_TRUE, 0,
			     m * n * sizeof(*c), c, 0, NULL, NULL);

  /* Call clblas extended function. Perform gemm for the lower right sub-matrices */
  err = clblasDgemm(clblasColumnMajor, transA, transB, m, n, k,
		    alpha, bufA, 0, lda,
		    bufB, 0, ldb, beta,
		    bufC, 0, ldc,
		    1, &queue, 0, NULL, &event);
  err = clblasDgemm(clblasColumnMajor, transA, transB, m, n, k,
		    alpha, bufA, 0, lda,
		    bufB, 0, ldb, beta,
		    bufC, 0, ldc,
		    1, &queue, 0, NULL, &event);
  err = clblasDgemm(clblasColumnMajor, transA, transB, m, n, k,
		    alpha, bufA, 0, lda,
		    bufB, 0, ldb, beta,
		    bufC, 0, ldc,
		    1, &queue, 0, NULL, &event);
  err = clblasDgemm(clblasColumnMajor, transA, transB, m, n, k,
		    alpha, bufA, 0, lda,
		    bufB, 0, ldb, beta,
		    bufC, 0, ldc,
		    1, &queue, 0, NULL, &event);
  
  if (err != CL_SUCCESS) {
    printf("clblasDgemm() failed with %d\n", err);
    ret = 1;
  } else {
    /* Wait for calculations to be finished. */
    err = clWaitForEvents(1, &event);
    
    /* Fetch results of calculations from GPU memory. */
    err = clEnqueueReadBuffer(queue, bufC, CL_TRUE, 0,
			      m * n * sizeof(*c),
			      c, 0, NULL, NULL);
  }

  /* Release OpenCL memory objects. */
  clReleaseMemObject(bufC);
  clReleaseMemObject(bufB);
  clReleaseMemObject(bufA);

  /* Finalize work with clblas. */
  /*  clblasTeardown();*/

  /* Release OpenCL working objects. */
  clReleaseCommandQueue(queue);
  clReleaseContext(ctx);
}

int main( void)
{
  double *a, *b, *c1, *c2, err, t, gflops, alpha, beta;
  int i;
  struct timeval ts, te;
  
  clblasSetup();

  a = (double*)malloc( sizeof(double) * N * N);
  b = (double*)malloc( sizeof(double) * N * N);
  c1 = (double*)malloc( sizeof(double) * N * N);
  c2 = (double*)malloc( sizeof(double) * N * N);
  alpha = 1.0; beta = 0.0;

  for ( i = STEP; i <= N; i += STEP) {
    printf( "%d ", i);
    initMatrix( a, i, i);
    initMatrix( b, i, i);

    gettimeofday( &ts, NULL);
    dgemm( 'N', 'N', i, i, i, 1.0, a, i, b, i, 0.0, c1, i);
    gettimeofday( &te, NULL);
    t = ( (double)te.tv_sec + (double)te.tv_usec / 1000000.0)
      - ( (double)ts.tv_sec + (double)ts.tv_usec / 1000000.0);
    gflops = (i * (double)i * i * 2.0) / t / 1000000000.0;
    // gflops = 0.0; t = 0.0;
    printf( "%.2f %.2f ", t, gflops);
    
    gettimeofday( &ts, NULL);
    gpuDgemm( 'N', 'N', i, i, i, 1.0, a, i, b, i, 0.0, c2, i);
    gettimeofday( &te, NULL);
    t = ( (double)te.tv_sec + (double)te.tv_usec / 1000000.0)
      - ( (double)ts.tv_sec + (double)ts.tv_usec / 1000000.0);
    gflops = (i * (double)i * i * 2.0) / t / 1000000000.0 * 4.0;
    // gflops = 0.0; t = 0.0;
    printf( "%.2f %.2f ", t, gflops);
    
    err = calcErr( c1, c2, i, i);
    printf( "%f\n", err);
  }

  /* Finalize work with clblas. */
  clblasTeardown();

  return 0;
}
