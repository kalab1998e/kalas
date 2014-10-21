#include <sys/time.h>
#include <f77blas.h>
#include "matrix.h"
#include "kalas.h"
#include "kalasState.h"
#include "kadbg.h"

int main(void)
{
  KalasState *state;
  Matrix *a, *b, *c1, *c2;
  typeKind type = KALAS_FLOAT;
  int am, an, bm, bn, cm, cn;
  float falpha = 1.0, fbeta = 0.0;
  double dalpha = 1.0, dbeta = 0.0, e;
  char ta = 'N', tb = 'N';
  struct timeval t;
  double ts, te;

  state = kalasStateNew();
 
  for ( int i = 128; i <= 4096; i+=4) {
    am = an = bm = bn = cm = cn = i;
    a = matrixNew( am, an, type, 1);
    b = matrixNew( bm, bn, type, 1);
    c1 = matrixNew( cm, cn, type, 0);
    c2 = matrixNew( cm, cn, type, 0);
    
    gettimeofday( &t, NULL);
    ts = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
    switch (type) {
    case KALAS_FLOAT:
      sgemm_( &ta, &tb, &cm, &cn, &an, &falpha, a->elm, &an,
              b->elm, &bn, &fbeta,
              c1->elm, &cn);
      break;
    case KALAS_DOUBLE:
      dgemm_( &ta, &tb, &cm, &cn, &an, &dalpha, a->elm, &an,
              b->elm, &bn, &dbeta,
              c1->elm, &cn);
      break;
    }
    gettimeofday( &t, NULL);
    te = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
    printf( "%d %f s %f GFlops ", am, (te - ts),
            (double)am * (double)am * (double)am * 2.0
            / ( te - ts) / 1000000000.0);

    gettimeofday( &t, NULL);
    ts = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
    switch (type) {
    case KALAS_FLOAT:
      kalasSgemm( state, 'N', 'N', cm, cn, an, 1.0, a->elm, an, b->elm, bn, 0.0,
                  c2->elm, cn);
      break;
    case KALAS_DOUBLE:
      kalasDgemm( state, 'N', 'N', cm, cn, an, 1.0, a->elm, an, b->elm, bn, 0.0,
                  c2->elm, cn);
      break;
    }
    gettimeofday( &t, NULL);
    te = t.tv_sec + (double)(t.tv_usec) / 1000000.0;
    printf( "%f s %f GFlops ", (te - ts),
            (double)am * (double)am * (double)am * 2.0
            / ( te - ts) / 1000000000.0);

    e = matrixCalcDiff( c1, c2);
    printf( "%e\n", e);

    matrixDelete(a);
    matrixDelete(b);
    matrixDelete(c1);
    matrixDelete(c2);
  }
  kalasStateDelete( state);
}

