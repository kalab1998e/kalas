#include <clBLAS.h>

typedef enum {
  hyblasRowMajor,
  hyblasColumnMajor
} hyblasOrder;

typedef enum {
  hyblasNoTrans,
  hyblasTrans
} hyblasTranspose;

typedef struct {
  cl_platform_id platform = 0;
  cl_device_id *device;
  cl_uint deviceNo;
  cl_context context = 0;
  cl_command_queue *commandQueue;
  RingQueueType ***bufQueue;
  unsigned long blockSize;
  unsigned long *noOfBlocks;
} HyBlasState;

bool HyBlasTest( void);

HyBlasState *HyBlasNew( void);

bool HyBlasDelete( HyBlasState *env);

/*
bool HyBlasSgemm( HyBlasState *env, HyBlasOrder order,
		  HyBlasTranspose TransA, HyBlasTranspose TransB,
		  int M, int N, int K,
		  float alpha, float *a, int LDA,
		  float *b, int LDB, float beta,
		  float *c, int LDC);
*/

bool HyBlasDgemm( HyBlasState *env, char TransA, char TransB,
		  int M, int N, int K,
		  double alpha, double *a, int LDA,
		  double *b, int LDB, double beta,
		  double *c, int LDC);
