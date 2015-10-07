#include <future.h>

int future_cons(future *fut) {
  int i, j;
  j = (int)fut;
  for (i=0; i<1000; i++) {
    j += i;
  }
  future_get(fut, j);
  return OK;
}


