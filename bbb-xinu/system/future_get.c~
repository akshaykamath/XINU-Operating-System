/* future_set.c - future_set */

#include <future.h>

/*if a thread calls future_get() on future in FUTURE_EMPTY, then the calling thread should block and its thread id should get stored in the pid field of the future. Now subsequent calls to future_get() on a future should fail with SYSERR*/
syscall future_get(future* futureRef,int* valueRef){
	
	if(!futureRef){
		printf("get: future has already been used by another consumer and/or is NULL \n");
		return SYSERR;
	}
	if (futureRef->state == FUTURE_EMPTY) 
	{		/* If caller must block	*/
		futureRef->state = FUTURE_WAITING;
		futureRef->pid = currpid;

		if(TestAndSet(futureRef) == 0)
		{
			printf("Process %d, lock acquired\n", currpid);
		}
		
		// while we have the lock, wait for the state to be updated
		while (TestAndSet(futureRef) == 1)
		{
			if (futureRef->state == FUTURE_VALID) 
			{
				*valueRef = futureRef->value; 
				futureRef->state = FUTURE_EMPTY;
				futureRef->pid = 0;	
				ReleaseLock(futureRef);	
				int status = future_free(&futureRef);
				return status;	
			}
		}
		//future_free(&futureRef);
	}
	
	if(futureRef->state == FUTURE_WAITING  || futureRef->state == FUTURE_VALID)
	{	
		return SYSERR;
	}

	return OK;
}
