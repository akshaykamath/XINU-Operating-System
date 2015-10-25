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
	 
	   kprintf("Process %d: future locked or non-existent\n", currpid);  

	   restore(mask);
	   return SYSERR;
	}


   intmask mask;
   mask = disable();
   kprintf("Process ID : %d consumed: %d\n",currpid, i);
   if(fut->flag != FUTURE_QUEUE)
   {
   	future_free(&fut);
   }

   restore(mask);
   return OK;
}



