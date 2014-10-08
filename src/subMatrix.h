#ifndef __SUB_MATRIX_H
#define __SUB_MATRIX_H
#include <CL/cl.h>
#include <clBLAS.h>
#include <stdbool.h>
#include "clState.h"
#include "ringQueue.h"

#define SUBMATRIX_MAX_SIZE 4
#define SUBMATRIX_MAX_SIZE2 SUBMATRIX_MAX_SIZE * SUBMATRIX_MAX_SIZE

typedef enum {
  SMFloat = 4, SMDouble = 8
} subMatrixFPType;

typedef enum {
  unknown, unmapped, mapped, running, finished
} SubMatrixCalcState;

typedef struct {
  CLState *state;
  unsigned int deviceNo;
  cl_event event[16];
  cl_mem *subMatA, *subMatB, *subMatC;
  subMatrixFPType type;
  unsigned int M, N, K;
  unsigned int blocks;
} SubMatrix;

void subMatrixTest( CLState *state);
SubMatrix *subMatrixNew( CLState *state, unsigned int no);
bool subMatrixClearDevice( SubMatrix *matrixs);
bool subMatrixWriteDevice( SubMatrix *matrixs, void *subA, void *subB);
bool subMatrixReadDevice( SubMatrix *matrixs, void *subC);
bool subMatrixCalc( SubMatrix *matrixs);
SubMatrixCalcState subMatrixCalcCheck( SubMatrix *matrixs);
void subMatrixDelete( SubMatrix *matrixs);

/*
typedef struct {
  unsigned int subMatCRow[JOBQUEUE_MAX_BLOCK_SIZE];
  unsigned int subMatCCol[JOBQUEUE_MAX_BLOCK_SIZE];
  unsigned int blockSize;
  void *subMatC[JOBQUEUE_MAX_BLOCK_SIZE2];
  RingQueue *subJob;
  JobState state;
} Job;

Job *jobNew( void);
void jobDelete( Job *job);
bool jobPushSubJob( Job *job, SubJob *sjob);
unsigned int jobGetSubJobLength( Job *job);
SubJob *jobGetSubJobAt( Job *job, const unsigned int loc);
*/

#endif
