#include "kalasState.h"
#include "clerr.h"
#include "debug.h"

void kalasStateTest( void)
{
  KalasState *state;

  TEST( !( state = kalasStateNew()));
  kalasStateDelete( state);

  fprintf( stderr, "clState class test finished.\n");
}

KalasState *kalasStateNew( void)
{
  KalasState *state;
  cl_context_properties props[3];
  cl_int err;

  if ( TEST( ! (state = (KalasState*)malloc( sizeof(KalasState))))){
    return NULL;
  }
  state->device = NULL;
  state->deviceNum = 0;
  state->queue = NULL;
  
  /* Setup OpenCL environment. */
  if ( TEST( ( err = clGetPlatformIDs(1, &(state->platform), NULL))
	     != CL_SUCCESS)) {
    fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err );
    kalasStateDelete( state);
    return NULL;
  }

  // 全てのGPUデバイスを取得する
  if ( TEST( ( err = clGetDeviceIDs( state->platform, CL_DEVICE_TYPE_GPU, 0,
				     NULL, &(state->deviceNum)))
	     != CL_SUCCESS)) {
    fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err);
    kalasStateDelete( state);
    return NULL;
  }
  if ( TEST( ! ( state->device =
		 (cl_device_id*)malloc( sizeof( cl_device_id)
					* state->deviceNum)))) {
    kalasStateDelete( state);
    return NULL;
  }
  if ( TEST( ( err = clGetDeviceIDs( state->platform, CL_DEVICE_TYPE_GPU,
				     state->deviceNum, state->device,
				     &(state->deviceNum))) != CL_SUCCESS)) {
    fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err);
    kalasStateDelete( state);
    return NULL;
  }

  // コンテキストの作成
  props[0] = CL_CONTEXT_PLATFORM;
  props[1] = (cl_context_properties)(state->platform);
  props[2] = 0;

  if ( TEST( (state->context =
	      clCreateContext( props, state->deviceNum, state->device,
			       NULL, NULL, &err), err)
	       != CL_SUCCESS)) {
    fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err);
    kalasStateDelete( state);
    return NULL;
  }

  // デバイス分のキューの作成
  if ( TEST( ( state->queue =
	       (cl_command_queue*)malloc( sizeof(cl_command_queue)
					  * state->deviceNum)) == NULL)) {
    kalasStateDelete( state);
    return NULL;
  }
  for ( int i = 0; i < state->deviceNum; i++)
    state->queue[ i] = NULL;
  for ( int i = 0; i < state->deviceNum; i++) {
    if ( TEST( ( state->queue[i] =
		 clCreateCommandQueue( state->context, state->device[i], 0,
					 &err), err)
		 != CL_SUCCESS)) {
      fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err);
      kalasStateDelete( state);
      return NULL;
    }
  }

  /* Setup clBLAS. */
  if ( TEST( ( err = clblasSetup()) != CL_SUCCESS)) {
    fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err);
    kalasStateDelete( state);
    return NULL;
  }

  return state;
}

void kalasStateDelete( KalasState *state)
{
  /* Release OpenCL working objects. */
  if ( state->queue != NULL)
    for ( int i = 0; i < state->deviceNum && state->queue[ i] != NULL;
	  i++)
      clReleaseCommandQueue( state->queue[i]);
      
  clReleaseContext( state->context);
  
  if ( state->queue) free( state->queue);
  
  if ( state->device) free( state->device);
  
  /* Finalize work with clBLAS. */
  clblasTeardown();

  return;
}
