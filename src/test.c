#include "ringQueue.h"
#include "clState.h"
#include "subMatrix.h"

int main( void)
{
  CLState *state;
  
  ringQueueTest();
  clStateTest();
  state = clStateNew();
  subMatrixTest( state);
  
  return 0;
}
