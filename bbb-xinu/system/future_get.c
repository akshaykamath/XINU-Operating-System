/* future_set.c - future_set */

#include <future.h>


/*if a thread calls future_get() on future in FUTURE_EMPTY, then the calling thread should block and its thread id should get stored in the pid field of the future. Now subsequent calls to future_get() on a future should fail with SYSERR*/
syscall future_get(future* futureRef,int* valueRef){

	if (futureRef->state == FUTURE_EMPTY) {		/* If caller must block	*/
		futureRef->state = FUTURE_WAITING;
		futureRef->pid = currpid;

		TestAndSet();

		while (TestAndSet());	
	}

	if (futureRef->state == FUTURE_VALID) {
		*valueRef = futureRef->value; 
		futureRef->state = FUTURE_EMPTY;
		futureRef->pid = 0;	
		return OK;
	}	
	
	if(futureRef->state == FUTURE_WAITING)
	{	
		return SYSERR;
	}

	return OK;
}

int TestAndSet() 
{    
bool oldval;    
oldval = lock;    
lock = true;    

return (oldval); 
}

void ReleaseLock()
{
lock = false;
}

