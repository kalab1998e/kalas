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
  typeKind type = KALAS_DOUBLE;
  int am, an, bm, bn, cm, cn;
  float falpha = 1.0, fbeta = 0.0;
  double dalpha = 1.0, dbeta = 0.0;
  char ta = 'N', tb = 'N';
  struct timeval t;
  double ts, te;

  state = kalasStateNew();
 
  for ( int i = 128; i <= 4096; i+=4) {
    am = an = bm = bn = cm = cn = i;
    a = matrixNew( am, an, type, 1);
    b = matrixNew( bm, bn, type, 1);
    c = matrixNew( cm, cn, type, 0);

    gettimeofday( &t, NULL);
    ts = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
    switch (type) {
    case KALAS_FLOAT:
      sgemm_( &ta, &tb, &cm, &cn, &an, &falpha, a->elm, &an,
              b->elm, &bn, &fbeta,
              c->elm, &cn);
      break;
    case KALAS_DOUBLE:
      dgemm_( &ta, &tb, &cm, &cn, &an, &dalpha, a->elm, &an,
              b->elm, &bn, &dbeta,
              c->elm, &cn);
      break;
    }
    gettimeofday( &t, NULL);
    te = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
    printf( "size: %d, time: %f s, speed: %f GFlops \t", am, (te - ts),
            (double)am * (double)am * (double)am * 2.0
            / ( te - ts) / 1000000000.0);

    gettimeofday( &t, NULL);
    ts = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
    switch (type) {
    case KALAS_FLOAT:
      kalasSgemm( state, 'N', 'N', cm, cn, an, 1.0, a->elm, an, b->elm, bn, 0.0,
                  c->elm, cn);
      break;
    case KALAS_DOUBLE:
      kalasDgemm( state, 'N', 'N', cm, cn, an, 1.0, a->elm, an, b->elm, bn, 0.0,
                  c->elm, cn);
      break;
    }
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

