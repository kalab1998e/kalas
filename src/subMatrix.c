#include <stdlib.h>
#include <CL/cl.h>
#include "clerr.h"
#include "debug.h"
#include "ringQueue.h"
#include "subMatrix.h"
#include "clState.h"

void subMatrixTest( CLState *state)
{
  SubMatrix *matrixs = NULL;
  cl_mem bufA[4] = { NULL, NULL, NULL, NULL};
  cl_mem bufB[4] = { NULL, NULL, NULL, NULL};
  cl_mem bufC[16] = { NULL, NULL, NULL, NULL, 
		      NULL, NULL, NULL, NULL, 
		      NULL, NULL, NULL, NULL, 
		      NULL, NULL, NULL, NULL};
  double *a = NULL, *b = NULL, *c = NULL;
  double *subA = NULL, *subB = NULL, *subC = NULL;

  // Prepare matrixs
  if ( TEST( a)) return;
  if ( TEST( ( a = malloc( sizeof( double) * 8192 * 8192)) == NULL)) return;
  if ( TEST( b)) return;
  if ( TEST( ( b = malloc( sizeof( double) * 8192 * 8192)) == NULL)) return;
  for ( int i = 0; i < 8192 * 8192; i++) {
    a[ i] = (double)rand() / RAND_MAX;
    b[ i] = (double)rand() / RAND_MAX;
  }

  if ( TEST( subA)) return;
  if ( TEST( ( subA = malloc( sizeof( double) * 4096 * 4096))
	     == NULL)) return;
  if ( TEST( subB)) return;
  if ( TEST( ( subB = malloc( sizeof( double) * 4096 * 4096))
	     == NULL)) return;

  for ( int row = 0; row < 4096; row++) {
    for ( int col = 0; col < 4096; col++) {
      subA[row * 4096 + col] = a[ row * 8192 + col];
      subB[row * 4096 + col] = b[ row * 8192 + col];
    }
  }

  if ( TEST( ! ( matrixs = subMatrixNew( state, 0)))) return;
  matrixs->type = SMDouble;
  matrixs->M = 4096;
  matrixs->N = 4096;
  matrixs->K = 4096;
  matrixs->blocks = 1;

  puts( "check1");
  if ( TEST( ! subMatrixClearDevice( matrixs))) return;
  puts( "check2");
  if ( TEST( ! subMatrixWriteDevice( matrixs, (void*)subA, (void*)subB)))
    return;
  puts( "check3");
  if ( TEST( ! subMatrixCalc( matrixs))) return;
  puts( "check4");
  while ( subMatrixCalcCheck( matrixs) != finished) ;
  puts( "check5");
  if ( TEST( ! subMatrixReadDevice( matrixs, (void*)subC))) return;
  subMatrixDelete( matrixs);
  puts( "check6");  
  fprintf( stderr, "SubMatrix class test finished\n");
}
  
SubMatrix *subMatrixNew( CLState *state, unsigned int no)
{
  SubMatrix *matrixs = NULL;

  if ( TEST( ! ( matrixs = (SubMatrix*)malloc(sizeof(SubMatrix))))) {
    return NULL;
  }

  matrixs->state = state;
  matrixs->deviceNo = no;
  matrixs->subMatA = matrixs->subMatB = matrixs->subMatC = NULL;
  matrixs->M = matrixs->N = matrixs-> K = 0;
  matrixs->type = SMFloat;
  matrixs->blocks = 0;

  return matrixs;
}

bool subMatrixClearDevice( SubMatrix *mat)
{
  void *dummy = NULL;
  cl_int err;

  // dummy != NULL is NG, alloced dummy == NULL is NG
  puts( "subMatrixClearDevice check1");
  if ( TEST( dummy)) return false;
  if ( TEST( ! ( dummy = (char*)calloc( mat->type, mat->M * mat->N))))
    return false;

  // alloced subMatC is NG.
  if ( mat->subMatC) return false;
  
  puts( "subMatrixClearDevice check2");  
  for ( int i = 0; i < mat->blocks * mat->blocks; i++) {
    puts( "subMatrixClearDevice check2-1");
    
    puts( "subMatrixClearDevice check2-2");
    if ( TEST( ( mat->subMatC[ i] = clCreateBuffer( mat->state->context,
						    CL_MEM_COPY_HOST_PTR,
						    mat->type * mat->M * mat->N,
						    NULL, &err),
		 err) != CL_SUCCESS)) {
      puts( "subMatrixClearDevice check2-2-1");
      fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err );
      puts( "subMatrixClearDevice check2-2-2");
      if ( ! dummy) return false;
      puts( "subMatrixClearDevice check2-2-3");
      free( dummy); dummy = NULL;
      puts( "subMatrixClearDevice check2-2-4");
      return false;
    }
    puts( "subMatrixClearDevice check2-3");
    if ( TEST( ( err = clEnqueueWriteBuffer( mat->state->queue[mat->deviceNo],
					     mat->subMatC[ i], CL_TRUE, 0,
					     mat->M * mat->N * mat->type,
					     dummy, 0, NULL, NULL), err)
	       != CL_SUCCESS)) {
      fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err );

      if ( TEST( ! dummy)) return false;
      free( dummy); dummy = NULL;
      return false;
    }
  }

  puts( "subMatrixClearDevice check3");
  if ( TEST( ! dummy)) return false;
  free( dummy); dummy = NULL;
  return true;
}

bool subMatrixWriteDevice( SubMatrix *mat, void *subA, void *subB)
{
  cl_int err;
  
  int sizeA = mat->M * mat->K * mat->type;
  int sizeB = mat->N * mat->K * mat->type;
  
  if ( mat == NULL) return false;
  
  for ( int i = 0; i < mat->blocks; i++) {

    if ( TEST( mat->subMatA[i])) return false;
    if ( TEST( ( mat->subMatA[i] = clCreateBuffer( mat->state->context,
						   CL_MEM_COPY_HOST_PTR,
						   sizeA, NULL, &err), err)
	       != CL_SUCCESS)) {
      fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err );
      return false;
    }
	 
    if ( TEST( mat->subMatB[i])) return false;
    if ( TEST( ( mat->subMatB[i] = clCreateBuffer( mat->state->context,
						   CL_MEM_COPY_HOST_PTR,
						   sizeB, NULL, &err), err) 
	       != CL_SUCCESS)) {
      fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err );
      return false;
    }
    if ( TEST( ( err = clEnqueueWriteBuffer(mat->state->queue[mat->deviceNo],
					    mat->subMatA[i],
					    CL_TRUE, 0, sizeA,
					    subA + i * sizeA, 0,
					    NULL, NULL)) != CL_SUCCESS)) {
      fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err );
      return false;
    }
    if ( TEST( ( err = clEnqueueWriteBuffer(mat->state->queue[mat->deviceNo],
					    mat->subMatB[i],
					    CL_TRUE, 0, sizeB,
					    subB + i * sizeB, 0,
					    NULL, NULL)) != CL_SUCCESS)) {
      fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err );
      return false;
    }
  }

  return true;
}

bool subMatrixReadDevice( SubMatrix *mat, void *subC)
{
  unsigned int sizeC = mat->M * mat->N * mat->type;
  cl_int err;
  
  if ( ! subC) return false;
  if ( ! mat) return false;

  for ( int i = 0; i < mat->blocks * mat->blocks; i++) {
    if ( ! mat->subMatC[i]) return false;
    
    if ( TEST( ( err = clEnqueueReadBuffer(mat->state->queue[mat->deviceNo],
					   mat->subMatC[i], CL_TRUE, 0, sizeC,
					   subC + i * sizeC, 0,
					   NULL, NULL)) != CL_SUCCESS)) {
      fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err );
      return false;
    }
  }

  return true;
}

bool subMatrixCalc( SubMatrix *matrixs)
{
  cl_int err;

  if ( ! matrixs) return false;
  
  for ( int i = 0; i < matrixs->blocks; i++) {
    for ( int j = 0; j < matrixs->blocks; j++) {
      int idx = i * matrixs->blocks + j;
      if ( TEST( ( err = clblasDgemm( clblasColumnMajor,
				      clblasNoTrans, clblasNoTrans,
				      matrixs->M, matrixs->N, matrixs->K,
				      1.0, matrixs->subMatA[i], 0, matrixs->K,
				      matrixs->subMatB[j], 0, matrixs->N, 0.0,
				      matrixs->subMatC[idx], 0, matrixs->N,
				      1, matrixs->state->queue,
				      0, NULL, &(matrixs->event[idx])), err)
		 != CL_SUCCESS)) {
	fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err );
	return false;
      }
    }
  }

  return true;
}

SubMatrixCalcState subMatrixCalcCheck( SubMatrix *matrixs)
{
  cl_int err, val;
  SubMatrixCalcState s = finished;
  
  for ( int i = 0; i < matrixs->blocks * matrixs->blocks; i++) {
    if ( TEST( ( err = clGetEventInfo( matrixs->event[i],
				       CL_EVENT_COMMAND_EXECUTION_STATUS,
				       sizeof( val), &val, NULL), err)
	       != CL_SUCCESS)) {
      fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err );
      return unknown;
    }
    
    if ( val != CL_COMPLETE) s = running;
  }

  return s;
}

void subMatrixDelete( SubMatrix *matrixs)
{
  cl_int err;
  
  if ( ! matrixs) {
    if ( ! matrixs->subMatA) {
      for ( int i = 0; i < matrixs->blocks; i++) {
	if ( TEST( (err = clReleaseMemObject( matrixs->subMatA[i]))
		   != CL_SUCCESS)) {
	  fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err );
	}
      }
    }

    if ( ! matrixs->subMatB) {
      for ( int i = 0; i < matrixs->blocks; i++) {
	if ( TEST( ( err = clReleaseMemObject( matrixs->subMatB[i]))
		   != CL_SUCCESS)) {
	  fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err );
	}
      }
    }
	  
    if ( ! matrixs->subMatC) {
      for ( int i = 0; i < matrixs->blocks * matrixs->blocks; i++) {
	if ( TEST( ( err = clReleaseMemObject( matrixs->subMatC[i]))
		   != CL_SUCCESS)) {
	  fprintf( stderr, "%s(%d)\n", clErrNo2Str( err), err );
	}
      }
    }
  }
  
  if ( ! matrixs->subMatA) {
    for ( int i = 0; i < matrixs->blocks; i++)
      clReleaseMemObject( matrixs->subMatA[i]);
  }
  
  if ( ! matrixs->subMatB) {
    for ( int i = 0; i < matrixs->blocks; i++)
      clReleaseMemObject( matrixs->subMatB[i]);
  }

  free( matrixs);
}
/*
Job *jobNew( void)
{
  Job *job = NULL;
  
  TEST( ! ( job = (Job*)malloc(sizeof(Job))));
  TEST( ! ( job->subJob = ringQueueNew()));
  job->blockSize = 0;
  job->state = unmapped;
  for ( int i = 0; i < JOBQUEUE_MAX_BLOCK_SIZE2; i++) {
    job->subMatC[ i] = NULL;
  }
  return job;
}

void jobDelete( Job *job)
{
  SubJob *sjob;
  cl_int err;
  int i;
  
  if ( ! job) {
    for ( i = 0; i < JOBQUEUE_MAX_BLOCK_SIZE2; i++) {
      if ( ! job->subMatC[i]) free( job->subMatC[i]);
    }

    while ( sjob = (SubJob*)ringQueuePopEnd( job->subJob)) {
      subJobDelete( sjob);
    }
    free( job);
  }
}
 
bool jobPushSubJob( Job *job, SubJob *sjob)
{
  return ringQueuePushEnd( job->subJob, (void*)sjob);
}

unsigned int jobGetSubJobLength( Job *job)
{
  return ringQueueGetLength( job->subJob);
}

SubJob *jobGetSubJobAt( Job *job, const unsigned int loc)
{
  return (SubJob*)ringQueueGetAt( job->subJob, loc);
}
*/
