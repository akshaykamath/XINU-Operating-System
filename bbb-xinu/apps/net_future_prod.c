#include <future.h>
#include <udpsend.h>

uint32 net_prod(future *fut) {
  int  j,status;

  if(!fut){
	intmask mask;
	mask = disable();
	kprintf("fprod : future is NULL\n");
	restore(mask);
	return SYSERR;
  }
  

   if(fut->flag == FUTURE_QUEUE)
   {
   	  intmask mask;
	  mask = disable();	
	  j = udpsend();
	  
	  status = future_set(fut, &j);
	  kprintf("currpid :%d produced: %d, future set = %d\n",currpid, j, fut->value);
	  restore(mask);
   }
   else
   {
	j = udpsend();
	  
	status = future_set(fut, &j);
  }	 

  if(status == SYSERR){
	intmask mask;
	mask = disable();
	kprintf("fprod :future is either null or empty state or already in valid state\n");
	restore(mask);
  }
  return OK;
}


