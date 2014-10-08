#include "ringQueue.h"

void ringQueueTest( void)
{
  RingQueue *q;
  int x, y;

  x = rand(); y = rand();
  TEST( ! ( q = ringQueueNew()));
  TEST( ringQueuePopBegin( q));
  TEST( ringQueuePopEnd( q));
  
  TEST( ! ringQueuePushEnd( q, &x));
  TEST( ! ringQueuePushBegin( q, &y));

  TEST( ringQueueGetLength( q) != 2);
  
  TEST( (int*)ringQueueGetAt( q, 0) != &y);
  TEST( (int*)ringQueueGetAt( q, 1) != &x);
  TEST( ringQueueGetAt( q, 2) != NULL);
  TEST( (int*)ringQueuePopBegin( q) != &y);
  TEST( (int*)ringQueuePopEnd( q) != &x);

  // 容量溢れのテスト
  for ( int i = 0; i < 500; i++) TEST( ! ringQueuePushEnd( q, (void*)&x));
  for ( int i = 0; i < 500; i++) TEST( (int*)ringQueuePopEnd( q) != &x);
  for ( int i = 0; i < 500; i++) TEST( ! ringQueuePushBegin( q, (void*)&x));
  for ( int i = 0; i < 500; i++) TEST( (int*)ringQueuePopBegin( q) != &x);

  // リングのテスト
  for ( int i = 0; i < 800; i++) TEST( ! ringQueuePushEnd( q, (void*)&x));
  for ( int i = 0; i < 500; i++) TEST( (int*)ringQueuePopBegin( q) != &x);
  for ( int i = 0; i < 500; i++)
    TEST( ! (int*)ringQueuePushEnd( q, (void*)&x));
  for ( int i = 0; i < 800; i++) TEST( (int*)ringQueuePopBegin( q) != &x);
  
  for ( int i = 0; i < 800; i++) TEST( ! ringQueuePushBegin( q, (void*)&x));
  for ( int i = 0; i < 500; i++) TEST( (int*)ringQueuePopEnd( q) != &x);
  for ( int i = 0; i < 500; i++)
    TEST( ! ringQueuePushBegin( q, (void*)&x));
  for ( int i = 0; i < 800; i++) TEST( (int*)ringQueuePopEnd( q) != &x);

  TEST( ! ringQueueDelete( q));
  
  puts( "ringQueue class test finished.");

  return;
}

RingQueue *ringQueueNew( void)
{
  RingQueue *q;

  if (TEST(( q = (RingQueue*)malloc( sizeof( RingQueue))) == NULL))
    return NULL;

  q->length = 128; // 最初の要素数の長さは128とする．
  if (TEST(( q->val = (void**)malloc( sizeof(void*) * q->length)) == NULL))
    return NULL;

  for ( int i = 0; i < q->length; i++) q->val[ i] = NULL;
  q->begin = 0; q->end = 0;
  return q;
}

bool ringQueueDelete( RingQueue *q)
{
  if ( ! q) {
    if ( ! q->val) free( q->val);
    free( q);
  }
  return true;
}

bool ringQueuePushBegin( RingQueue *q, void *val)
{
  int loc;
  
  loc = ( q->begin - 1 + q->length) % q->length;

  // 記憶領域が足りない場合
  if ( q->end == loc) {
    if ( ! ringQueueIncreaseCapacity( q)) {
      return false;
    }
  }

  q->begin = loc;
  (q->val)[ q->begin] = val;
  return true;
}

bool ringQueuePushEnd( RingQueue *q, void *val)
{
  // 記憶領域が足りない場合
  if ( q->begin == ( q->end + 1) % q->length) {
    if ( ! ringQueueIncreaseCapacity( q)) {
      return false;
    }
  }
  
  (q->val)[ q->end] = val;
  q->end = ( q->end + 1) % q->length;
  return true;
}

void *ringQueuePopBegin( RingQueue *q)
{
  void *ret;
  
  // quene has no element
  if ( q->begin == q->end) {
    return NULL;
  }

  ret = (q->val)[ q->begin];
  (q->val)[ q->begin] = NULL;
  q->begin = ( q->begin + 1) % q->length;
  return ret;
}

void *ringQueuePopEnd( RingQueue *q)
{
  void *ret;
  
  // quene has no value
  if ( q->begin == q->end) {
    return NULL;
  }

  q->end = ( q->end - 1 + q->length) % q->length;
  ret = (q->val)[ q->end];
  (q->val)[ q->end] = NULL;
  return ret;
}

void *ringQueueGetAt( const RingQueue *q, const unsigned int i)
{
  if ( i >= q->length) return NULL;
  if ( q->begin == q->end && (q->val)[ q->begin] == NULL) return NULL;
  if ( ringQueueGetLength( q) < i) return NULL;
  return (q->val)[ ( q->begin + i) % q->length];
}

unsigned int ringQueueGetLength( const RingQueue *q)
{
  return ( q->end - q->begin + q->length) % q->length;
}

bool ringQueueIncreaseCapacity( RingQueue *q)
{
  void **newval = NULL;
  int i, j, l;

  // get memory space 2 times
  if ( TEST( ! (newval = malloc( sizeof(void*) * (q->length)*2)))) {
    return false;
  }

  // copy from values of Queue to new spaces
  j = q->begin;
  for ( i = 0; i < q->length - 1; i++) {
    newval[i] = (q->val)[j];
    j = ( j + 1) % q->length;
  }
  q->begin = 0;
  q->end = i;
  q->length = q->length * 2;
  free( q->val);
  q->val = newval;

  // left spaces are setted NULL
  for ( ; i < q->length; i++) {
    q->val[ i] = NULL;
  }
  return true;
}
