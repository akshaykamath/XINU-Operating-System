#include <future.h>

bool lock1;

int TestAndSet1() 
{    
bool oldval;    
oldval = lock1;    
lock1 = true;    

return (oldval); 
}

void ReleaseLock1()
{
lock1 = false;
}

uint32 future_cons(future *fut) {
  int i, status;
  status = future_get(fut, &i);
 
  if (status == SYSERR) 
	{	
		while (TestAndSet1());	
		
		printf("Process %d Error future locked by process: %d\n", currpid, fut->pid);		

		ReleaseLock1();
		
	    return SYSERR;
	}

   while (TestAndSet1());	
	
   printf("Process %d consumed %d\n",currpid, i);  

   ReleaseLock1();
   return OK;
}



