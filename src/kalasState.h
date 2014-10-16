#ifndef KALAS_STATE_H__
#define KALAS_STATE_H__

#include <clBLAS.h>

typedef struct {
  cl_platform_id platform;
  cl_device_id *device;
  cl_uint deviceNum;
  cl_context context;
  cl_command_queue *queue;
} KalasState;

void kalasStateTest( void);
KalasState *kalasStateNew( void);
void kalasStateDelete( KalasState *state);

#endif // KALAS_STATE_H__
