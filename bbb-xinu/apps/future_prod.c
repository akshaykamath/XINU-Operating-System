#include <future.h>

uint32 future_prod(future *fut) {
  int i, j;
  j = fut->value;
  for (i=0; i<1000; i++) {
    j += i;
  }
  future_set(fut, &j);
  return OK;
}


