#include <future.h>

uint32 future_cons(future *fut) {
  int i, status;
  status = future_get(fut, &i);
 
  if (status == SYSERR) 
	{	
            printf("Process %d Error future locked by process: %d\n", currpid, fut->pid);	
	    return SYSERR;
	}

   printf("Process %d consumed %d\n",currpid, i);  
   return OK;
}



