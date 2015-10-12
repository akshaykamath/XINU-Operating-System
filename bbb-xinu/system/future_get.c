/* future_set.c - future_set */

#include <future.h>

/*if a thread calls future_get() on future in FUTURE_EMPTY, then the calling thread should block and its thread id should get stored in the pid field of the future. Now subsequent calls to future_get() on a future should fail with SYSERR*/
syscall future_get(future* futureRef,int* valueRef){

	if (futureRef->state == FUTURE_EMPTY) 
	{		/* If caller must block	*/
		futureRef->state = FUTURE_WAITING;
		futureRef->pid = currpid;

		if(TestAndSet() == 0)
		{
			printf("Process %d, lock acquired\n", currpid);
		}
		
		// while we have the lock, wait for the state to be updated
		while (TestAndSet() == 1)
		{
			if (futureRef->state == FUTURE_VALID) 
			{
				*valueRef = futureRef->value; 
				futureRef->state = FUTURE_EMPTY;
				futureRef->pid = 0;	
				ReleaseLock();	
				break;	
			}
		}
	}
	
	if(futureRef->state == FUTURE_WAITING)
	{	
		return SYSERR;
	}

	return OK;
}
