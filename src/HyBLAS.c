#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "ringQueue.h"
#include "HyBLAS.h"
#include "clerr.h"
#include "debug.h"

bool HyBlasTest( void)
{
  return true;
}

HyBlasState *HyBlasNew( void)
{
  HyBlasState *env;
  cl_context_properties props[3];
  cl_int err;
  cl_ulong *allocSize;
  cl_ulong *globalSize;

  if ( ! TEST( env = (HyBlasState*)malloc( sizeof(HyBlasState)) != NULL)) {
    return NULL;
  }
  env->device = NULL;
  env->deviceNo = 0;
  env->cl_commandQueue = NULL;
  env->bufQueue = NULL;
  env->noOfBlocks = NULL;
  
  /* Setup OpenCL environment. */
  if ( ! TEST( ( err = clGetPlatformIDs(1, &platform, NULL)) == CL_SUCCESS)) {
    fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err );
    HyBlasDelete( env);
    return NULL;
  }

  // 全てのGPUデバイスを取得する
  if ( ! TEST( ( err = clGetDeviceIDs( env->platform, CL_DEVICE_TYPE_GPU, 0,
				       NULL, &(env->deviceNo)))
	       == CL_SUCCESS)) {
    fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err);
    HyBlasDelete( env);
    return NULL;
  }
  if ( ! TEST( ( env->device = (cl_device_type*)malloc( sizeof( cl_device_type)
							* env->deviceNo))
	       != NULL)) {
    fprintf( stderr, "Out of memory\n");
    HyBlasDelete( env);
    return NULL;
  }
  if ( ! TEST( ( err = clGetDeviceIDs( env->platform, CL_DEVICE_TYPE_GPU,
				       env->deviceNo, cl->device,
				       &(env->deviceNo))) == CL_SUCCESS)) {
    fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err);
    HyBlasDelete( env);
    return NULL;
  }

  // コンテキストの作成
  props = { CL_CONTEXT_PLATFORM, (cl_context_properties)(env->platform), 0};
  if ( ! TEST( (context = clCreateContext( props, env->deviceNo, env->device,
					   NULL, NULL, &err), err)
	       == CL_SUCCESS)) {
    fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err);
    HyBlasDelete( env);
    return NULL;
  }

  // デバイス分のキューの作成
  if ( ! TEST( ( env->queue = (cl_command_queue*)malloc( sizeof(cl_command_queue) * env->deviceNo)) != NULL)) {
    fprintf( stderr, "Out of memory\n");
    HyBlasDelete( env);
    return NULL;
  }
  for ( int i = 0; i < env->deviceNo; i++)
    env->commandQueue[ i] = (cl_command_queue*)NULL;
  for ( int i = 0; i < env->deviceNo; i++) {
    if ( ! TEST( ( env->queue[i] = clCreateCommandQueue( env->context,
							 env->device[i], 0,
							 &err), err)
		 == CL_SUCCESS)) {
      fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err);
      HyBlasDelete( env);
      return NULL;
    }
  }

  /* Setup clBLAS. */
  if ( ! TEST( ( err = clblasSetup()) != CL_SUCCESS)) {
    fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err);
    HyBlasDelete( env);
    return NULL;
  }

  /* block sizeの計算 */
  if ( !TEST( ( env->noOfBlocks =
		(unsigned int*)malloc( sizeof( unsigned int)
				       * env->deviceNo)) != NULL)) {
    HyBlasDelete( env);
    return NULL;
  }
  
  if ( !TEST( ( allocSize = (cl_ulong*)malloc( sizeof( cl_ulong)
					       * env->deviceNo)) != NULL)) {
    HyBlasDelete( env);
    return NULL;
  }
  if ( !TEST( ( globalSize = (cl_ulong*)malloc( sizeof( cl_ulong)
						* env->deviceNo)) != NULL)) {
    free( allocSize);
    HyBlasDelete( env);
    return NULL;
  }

  env->blockSize = ULONG_MAX;
  for ( int i = 0; i < env->deviceNo; i++) {
    if ( !TEST( ( err = clGetDeviceInfo( cl->device[ i],
					 CL_DEVICE_GLOBAL_MEM_SIZE,
					 sizeof(cl_ulong), &(globalSize[ i]),
					 NULL))
		== CL_SUCCESS)) {
      fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err);
      free( allocSize);
      free( globalSize);
      HyBlasDelete( env);
      return NULL;
    }

    if ( !TEST( ( err = clGetDeviceInfo( cl->device[ i],
					 CL_DEVICE_MAX_MEM_ALLOC_SIZE,
					 sizeof(cl_ulong), &(allocSize[ i]),
					 NULL))
		== CL_SUCCESS)) {
      fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err);
      free( allocSize);
      free( globalSize);
      HyBlasDelete( env);
      return NULL;
    }

    if ( env->blockSize > allocSize[ i]) env->blockSize = allocSize[ i];
  }

  for ( int i = 0; i < env->deviceNo; i++) {
    env->noOfBlocks[ i] = ( globalSize[ i] - 1) / env->blockSize
      + ( ( globalSize[ i] % env->blockSize) > 0);
    if ( env->noOfBlocks[ i] < 4) env->noOfBlocks[ i] = 4;
  }

  env->blockSize = MAX_ULONG;
  for ( int i = 0; i < env->deviceNo; i++)
    if ( env->blockSize > globalSize[ i] / env->noOfBlocks[ i])
      env->blockSize = globalSize[ i] / env->noOfBlocks[ i];

  for ( int i = 0; i < env->deviceNo; i++) {
    env->noOfBlocks[ i] = ( globalSize[ i] - 1) / env->blockSize
      + ( ( globalSize[ i] % env->blockSize) > 0);
    if ( env->noOfBlocks[ i] < 4) env->noOfBlocks[ i] = 4;
  }
  
  free( globalSize);
  free( allocSize);
  return env;
}

bool HyBlasDelete( HyBlasState *env)
{
  if ( env->noOfBlocks != NULL) free( env->noOfBlocks);

  /* Release OpenCL working objects. */
  if ( env->commandQueue != NULL)
    for ( int i = 0; i < deviceNo && env->commandQueue[ i] != NULL; i++)
      clReleaseCommandQueue( env->commandQueue[i]);
      
  clReleaseContext(env->context);
  
  if ( env->commandQueue != NULL) free( commandQueue);
  
  if ( env->device != NULL) free( device);
  
  /* Finalize work with clBLAS. */
  clblasTeardown();

  return TRUE;
}
 

int HyBlasDgemm( HyBlasState *env, char TransA, char TransB,
		  int m, int n, int k,
		  double alpha, double *a, int LDA,
		  float *b, int LDB, double beta,
		  double *c, int LDC)
{
  unsigned int baRow, baCol, bbRow, bbCol, bcRow, bcCol;
  HyBlasXgemmState *state;

  // ブロック化した時の行列に必要な大きさ
  baCol = k;
  if ( !TEST( ( baRow = env->blockSize / baCol / sizeof( double)) > 0))
    return FALSE;
  bbRow = k;
  if ( !TEST( ( bbCol = env->blockSize / bbRow / sizeof( double)) > 0))
    return FALSE;
  bcRow = baRow;
  bcCol = bbCol;
  if ( !TEST( ( bcRow * bcCol * sizeof( double)) <= env->blockSize))
    return FALSE;

  if ( !TEST( ( state = (HyBlasXgemmState*)malloc( sizeof(HyBlasXgemmState)
						   * env->blockSize))))
    return FALSE;

  
  free( state);
  return TRUE;
}
