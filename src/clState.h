#ifndef __CL_STATE_H
#define __CL_STATE_H

#include <clBLAS.h>

typedef struct {
  cl_platform_id platform;
  cl_device_id *device;
  cl_uint deviceNum;
  cl_context context;
  cl_command_queue *queue;
} CLState;

void clStateTest( void);
CLState *clStateNew( void);
void clStateDelete( CLState *state);

#endif // __CL_STATE_H
