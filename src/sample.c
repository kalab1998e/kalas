#include <sys/time.h>
#include <f77blas.h>
#include "matrix.h"
#include "kalas.h"
#include "kalasState.h"
#include "kadbg.h"

int main(void)
{
  KalasState *state;
  Matrix *a, *b, *c;
  int am, an, bm, bn, cm, cn;
  float alpha = 1.0, beta = 0.0;
  char ta = 'N', tb = 'N';
  struct timeval t;
  double ts, te;

  state = kalasStateNew();
 
  for ( int i = 128; i <= 4096; i+=4) {
    am = an = bm = bn = cm = cn = i;
    a = matrixNew( am, an, KALAS_FLOAT, 1);
    b = matrixNew( bm, bn, KALAS_FLOAT, 1);
    c = matrixNew( cm, cn, KALAS_FLOAT, 0);

    gettimeofday( &t, NULL);
    ts = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
    sgemm_( &ta, &tb, &cm, &cn, &an, &alpha, a->elm, &an, b->elm, &bn, &beta,
                c->elm, &cn);
    gettimeofday( &t, NULL);
    te = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
    printf( "size: %d, time: %f s, speed: %f GFlops \t", am, (te - ts),
            (double)am * (double)am * (double)am * 2.0
            / ( te - ts) / 1000000000.0);

    gettimeofday( &t, NULL);
    ts = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
    kalasSgemm( state, 'N', 'N', cm, cn, an, 1.0, a->elm, an, b->elm, bn, 0.0,
                c->elm, cn);
    gettimeofday( &t, NULL);
    te = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
    printf( "size: %d, time: %f s, speed: %f GFlops\n", am, (te - ts),
            (double)am * (double)am * (double)am * 2.0
            / ( te - ts) / 1000000000.0);

    matrixDelete(a);
    matrixDelete(b);
    matrixDelete(c);

    
  }
  kalasStateDelete( state);
}

