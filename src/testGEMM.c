#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <malloc.h>
#include <time.h>
#include <acml.h>
#include <math.h>
#include "myBLAS.h"

int main( int argc, char *argv[])
{
  float *finput1, *finput2, *foutput1, *foutput2;
  double *dinput1, *dinput2, *doutput1, *doutput2;
  unsigned int idxA, idxB, idxC, row, col, i;
  unsigned int MS, MS2, BS, BS2;
  float fs, fv;
  double ds, dv, t;
  clock_t ts, te;

  // myBlasを使うための初期化
  myBlasInitialize();

  // floatテスト
  // テスト行列の用意
  MS = 1024; MS2 = MS * MS;
  if ( ( finput1 = (float*)malloc( sizeof(float) * MS2)) == NULL) {
    fprintf( stderr, "Out of memory for finput1\n");
    abort();
  }
  if ( ( finput2 = (float*)malloc( sizeof(float) * MS2)) == NULL) {
    fprintf( stderr, "Out of memory for finput2\n");
    abort();
  }
  if ( ( foutput1 = (float*)malloc( sizeof(float) * MS2)) == NULL) {
    fprintf( stderr, "Out of memory for foutput1\n");
    abort();
  }
  if ( ( foutput2 = (float*)malloc( sizeof(float) * MS2)) == NULL) {
    fprintf( stderr, "Out of memory for foutput2\n");
    abort();
  }

  srand(0);
  for ( i = 0; i < MS2; i++) {
    finput1[ i] = rand() / (float)RAND_MAX;
    finput2[ i] = rand() / (float)RAND_MAX;
    foutput1[ i] = foutput2[ i] = 0.0f;
  }
  
  ts = clock();
  mySgemm( 'N', 'N', MS, MS, MS, 1.0f, finput1, MS, finput2, MS, 0.0f,
	   foutput1, MS);
  te = clock();
  t = (double)(te-ts) / CLOCKS_PER_SEC;
  printf( "%7.2e %7.2e ", t, (2.0*MS2*MS) / t / 1000000000.0);
  
  ts = clock();
  sgemm( 'N', 'N', MS, MS, MS, 1.0f, finput2, MS, finput1, MS, 0.0f,
	 foutput2, MS);
  te = clock();
  t = (double)(te-ts) / CLOCKS_PER_SEC;
  printf( "%7.2e %7.2e ", t, (2.0*MS2*MS) / t / 1000000000.0);

  // 誤差の算出
  fs = fv = 0.0f;
  for ( i = 0; i < MS2; i++) {
    fs = fs + ( foutput1[i] + foutput2[i]) / 2.0f;
    fv = fv + ( foutput1[i] - foutput2[i])
      * ( foutput1[i] - foutput2[i]);
  }
  fs = fs / (float)MS2;
  fv = fv / (float)MS2;
  printf( "%7.2e\n", sqrt( fv) / fs);
  fflush( stdout);
    
  // 行列メモリ領域の開放
  free( finput1);
  free( finput2);
  free( foutput1);
  free( foutput2);

  // doubleテスト
  // テスト行列の用意
  MS = 1024; MS2 = MS * MS;
  if ( ( dinput1 = (double*)malloc( sizeof(double) * MS2)) == NULL) {
    fprintf( stderr, "Out of memory for dinput1\n");
    abort();
  }
  if ( ( dinput2 = (double*)malloc( sizeof(double) * MS2)) == NULL) {
    fprintf( stderr, "Out of memory for dinput2\n");
    abort();
  }
  if ( ( foutput1 = (double*)malloc( sizeof(double) * MS2)) == NULL) {
    fprintf( stderr, "Out of memory for doutput1\n");
    abort();
  }
  if ( ( foutput2 = (double*)malloc( sizeof(double) * MS2)) == NULL) {
    fprintf( stderr, "Out of memory for doutput2\n");
    abort();
  }

  srand(0);
  for ( i = 0; i < MS2; i++) {
    dinput1[ i] = rand() / (double)RAND_MAX;
    dinput2[ i] = rand() / (double)RAND_MAX;
    doutput1[ i] = doutput2[ i] = 0.0;
  }
  
  ts = clock();
  myDgemm( 'N', 'N', MS, MS, MS, 1.0, dinput1, MS, dinput2, MS, 0.0,
	   doutput1, MS);
  te = clock();
  t = (double)(te-ts) / CLOCKS_PER_SEC;
  printf( "%7.2e %7.2e ", t, (2.0*MS2*MS) / t / 1000000000.0);
  
  ts = clock();
  dgemm( 'N', 'N', MS, MS, MS, 1.0, dinput2, MS, dinput1, MS, 0.0,
	 doutput2, MS);
  te = clock();
  t = (double)(te-ts) / CLOCKS_PER_SEC;
  printf( "%7.2e %7.2e ", t, (2.0*MS2*MS) / t / 1000000000.0);

  // 誤差の算出
  ds = dv = 0.0;
  for ( i = 0; i < MS2; i++) {
    ds = ds + ( doutput1[i] + doutput2[i]) / 2.0;
    dv = dv + ( doutput1[i] - doutput2[i])
      * ( doutput1[i] - doutput2[i]);
  }
  ds = ds / (double)MS2;
  dv = dv / (double)MS2;
  printf( "%7.2e\n", sqrt( fv) / fs);
  fflush( stdout);
    
  // 行列メモリ領域の開放
  free( dinput1);
  free( dinput2);
  free( doutput1);
  free( doutput2);

  // myBlasの終了処理
  myBlasFinalize();
  return 0;
}
