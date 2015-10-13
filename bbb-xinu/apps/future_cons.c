#include <future.h>

uint32 future_cons(future *fut) {
  int i, status;
  if(!fut){
   	intmask mask;
   	mask = disable();	
	kprintf("fcons : future is null\n");
	restore(mask);
	return SYSERR;
  }
  status = future_get(fut, &i);
  if (status == SYSERR) 
	{
	   intmask mask;
   	   mask = disable();
	   kprintf("Process %d Error future locked by process: %d\n", currpid, fut->pid);  
	   restore(mask);
	   return SYSERR;
	}

   intmask mask;
   mask = disable();
   kprintf("Process ID : %d\n",currpid);
   kprintf("it consumed : %d\n",i);
   restore(mask);
   return OK;
}



