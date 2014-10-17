#include <time.h>
#include "matrix.h"
#include "kalas.h"
#include "kalasState.h"
#include "kadbg.h"

int main(void)
{
  KalasState *state;
  Matrix *a, *b, *c;
  int am, an, bm, bn, cm, cn;
  clock_t cs, ce;
  double t;
  
  am = an = bm = bn = cm = cn = 1024;
  a = matrixNew( am, an, KALAS_FLOAT, 1);
  b = matrixNew( bm, bn, KALAS_FLOAT, 1);
  c = matrixNew( cm, cn, KALAS_FLOAT, 0);
  
  state = kalasStateNew();

  kalasSgemm( state, 'N', 'N', cm, cn, an, 1.0, a->elm, an, b->elm, bn, 0.0,
              c->elm, cn);
  
  kalasStateDelete( state);
}

