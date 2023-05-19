#pragma once

typedef struct _ringbuffer_t
{
  void* data;
  int element_size;
  int capacity;
  int write;
  int read;
} ringbuffer_t;

#ifdef TEST
void ringbuffer_tests() __fromfile("ringbuffer.c");
void ringbuffer_dump(ringbuffer_t*) __fromfile("ringbuffer.c");
#endif


void ringbuffer_init(ringbuffer_t*, unsigned* data, unsigned element_size, unsigned capacity) __fromfile("ringbuffer.c");

// Check if the ringbuffer is empty.
int ringbuffer_empty(ringbuffer_t*) __fromfile("ringbuffer.c");

// Push an element. Return TRUE if that potentially led to an overflow.
// We do not know this for sure, as at the same time a concurrent read
// could've made room, but without locking we can only assume a worst
// case
int ringbuffer_push(ringbuffer_t*, void* item) __fromfile("ringbuffer.c");

// Remove an element. Return TRUE if there were actual elements available.
// Similar to the push-case, we can only make a best guess at this due
// to concurrent writes going unnoticed.
int ringbuffer_pop(ringbuffer_t*, void* item) __fromfile("ringbuffer.c");

// Amount of stored elements
int ringbuffer_count(ringbuffer_t*) __fromfile("ringbuffer.c");

// Free count will maximum be up to capacity - 1.
// We sacrifice the last element to ensure we can determine
// empty correctly.
int ringbuffer_free(ringbuffer_t*) __fromfile("ringbuffer.c");

// A macro to simplify ringbuffer initialization based
// on type-inference.
// Pass an array of anything, and let both the overall size of the array
// as well as the size of one element determine the arguments.
#define RINGBUFFER_INIT(rb, data) \
  ringbuffer_init(rb, (void*)&data[0], sizeof(data[0]), sizeof(data) / sizeof(data[0]))
