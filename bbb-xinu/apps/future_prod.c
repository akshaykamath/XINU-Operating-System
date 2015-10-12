#include <future.h>

uint32 future_prod(future *fut) {
  int i, j,status;
	//printf("Test f : %d\n",fut);
  if(!fut){
	printf("fprod : future is NULL\n");
	return SYSERR;
  }
  
  j = fut->value;
  for (i=0; i<1000; i++) {
    j += i;
  }
  status = future_set(fut, &j);
  if(status == SYSERR){
	printf("fprod :future is either NULL or already in VALID state\n");
	return SYSERR;
  }
  return OK;
}


