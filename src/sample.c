#include <sys/time.h>
#include "matrix.h"
#include "kalas.h"
#include "kalasState.h"
#include "kadbg.h"

int main(void)
{
  KalasState *state;
  Matrix *a, *b, *c;
  int am, an, bm, bn, cm, cn;
  struct timeval t;
  double ts, te;
  
  state = kalasStateNew();
  
  for ( int i = 128; i <= 5792; i+=128) {
    am = an = bm = bn = cm = cn = i;
    a = matrixNew( am, an, KALAS_FLOAT, 1);
    b = matrixNew( bm, bn, KALAS_FLOAT, 1);
    c = matrixNew( cm, cn, KALAS_FLOAT, 0);

    gettimeofday( &t, NULL);
    ts = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
    kalasSgemm( state, 'N', 'N', cm, cn, an, 1.0, a->elm, an, b->elm, bn, 0.0,
                c->elm, cn);
    gettimeofday( &t, NULL);
    te = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
    printf( "size: %d, time: %f s, speed: %e GFlops\n", am, (te - ts),
            (double)am * (double)am * (double)am * 2.0
            / ( te - ts) / 1000000000.0);

    matrixDelete(a);
    matrixDelete(b);
    matrixDelete(c);
  }
  kalasStateDelete( state);
}

