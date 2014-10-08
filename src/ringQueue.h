#ifndef __RING_QUEUE_H
#define __RING_QUEUE_H

#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <stdbool.h>
#include "debug.h"

typedef struct {
  void **val;
  unsigned int begin, end, length;
} RingQueue;

void ringQueueTest( void);
RingQueue *ringQueueNew( void);
bool ringQueueDelete( RingQueue *q);
bool ringQueuePushBegin( RingQueue *q, void *val);
bool ringQueuePushEnd( RingQueue *q, void *val);
void *ringQueuePopBegin( RingQueue *q);
void *ringQueuePopEnd( RingQueue *q);
void *ringQueueGetAt( const RingQueue *q, const unsigned int i);
unsigned int ringQueueGetLength( const RingQueue *q);
bool ringQueueIncreaseCapacity( RingQueue *q);
#endif // __RING_QUEUE_H
