#include <future.h>

uint32 future_cons(future *fut) {
  int i, status;
  status = future_get(fut, &i);
 
  if (status == SYSERR) 
	{	
            printf("Process %d Error future locked by process: %d\n", currpid, fut->pid);	
	    return SYSERR;
	}

   printf("Process ID : %d\n",currpid);
   printf("it consumed : %d\n",i);
   future_free(fut);  
   return OK;
}



