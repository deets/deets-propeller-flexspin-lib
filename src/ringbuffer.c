#include "ringbuffer.h"
#include <assert.h>
#include <string.h>


void ringbuffer_init(ringbuffer_t* rb, unsigned* data, unsigned element_size, unsigned capacity)
{
  rb->data = data;
  rb->element_size = element_size;
  rb->capacity = capacity;
  rb->read = 0;
  rb->write = 0;
}

int ringbuffer_empty(ringbuffer_t* rb)
{
  return rb->read == rb->write;
}

int ringbuffer_push(ringbuffer_t* rb, const void* item)
{
  unsigned s = rb->element_size;
  unsigned read = rb->read;
  memcpy(rb->data + rb->write * s, item, s);
  // if we are empty after the write, we did overflow
  rb->write = (rb->write + 1) % rb->capacity;
  return read == rb->write;
}

int ringbuffer_pop(ringbuffer_t* rb, void* item)
{
  unsigned s = rb->element_size;
  int empty = rb->write == rb->read;
  memcpy(item, rb->data + rb->read * s, s);
  rb->read = (rb->read + 1) % rb->capacity;
  return !empty;
}

int ringbuffer_count(ringbuffer_t* rb)
{
  return (rb->write + rb->capacity - rb->read) % rb->capacity;
}

// Free count will maximum be up to capacity - 1.
// We sacrifice the last element to ensure we can determine
// empty correctly.
int ringbuffer_free(ringbuffer_t* rb)
{
  return rb->capacity - ringbuffer_count(rb) - 1;
}

#ifdef TEST
void ringbuffer_dump(ringbuffer_t*)
{
  //fds->str("-----\r\n");
}


void ringbuffer_tests()
{
  //fds->str("ringbuffer_tests:begin\r\n");
  ringbuffer_t rb;
  // Allocate some extra space to check for spillover
  unsigned data[4 + 4];
  memset(&data[0], 0xff, sizeof(data));
  unsigned item, another_item;
  ringbuffer_init(&rb, &data[0], sizeof(unsigned), (sizeof(data) >> 1) / sizeof(unsigned));
  assert(rb.capacity == 4);
  assert(ringbuffer_empty(&rb));

  item = 1000;
  assert(!ringbuffer_push(&rb, &item));
  assert(!ringbuffer_empty(&rb));
  assert(data[0] == 1000);
  assert(ringbuffer_pop(&rb, &another_item));
  assert(item == another_item);
  assert(rb.read == 1);
  assert(rb.write == 1);
  assert(!ringbuffer_push(&rb, &item));
  assert(data[1] == 1000);
  assert(ringbuffer_pop(&rb, &another_item));
  assert(item == another_item);

  // Write over our RB multiple times it's capacity
  for(int i = 0; i < rb.capacity * 2; ++i)
  {
    assert(!ringbuffer_push(&rb, &item));
    assert(ringbuffer_pop(&rb, &another_item));
    assert(item == another_item);
  }
  // Check for spillover
  assert(data[3] == 1000);
  assert(data[4] == 0xffffffff);

  rb.read = rb.write = 0;
  assert(ringbuffer_empty(&rb));
  assert(ringbuffer_count(&rb) == 0);
  assert(ringbuffer_free(&rb) == 3);
  assert(!ringbuffer_push(&rb, &item));
  assert(ringbuffer_count(&rb) == 1);

  // Now using the macro, we should
  // arrive at the full size of data. Rest
  // stays the same
  RINGBUFFER_INIT(&rb, data);
  assert(rb.capacity == 8);
  assert(rb.read == 0);
  assert(rb.write == 0);
  assert(rb.element_size == 4);
  assert(rb.data == &data[0]);

  printf("ringbuffer_tests:finished\r\n");
}

#endif
