#include <future.h>

uint32 future_prod(future *fut) {
  int i, j,status;

  if(!fut){
	intmask mask;
	mask = disable();
	kprintf("fprod : future is NULL\n");
	restore(mask);
	return SYSERR;
  }
  
	
  j = fut->value;
  for (i=0; i<10; i++) {
    j += i;
  }
  status = future_set(fut, &j);
  if(status == SYSERR){
	intmask mask;
	mask = disable();
	kprintf("fprod :future is either null or empty state or already in valid state\n");
	restore(mask);
  }
  return OK;
}


