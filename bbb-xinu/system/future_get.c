/* future_set.c - future_set */

#include <future.h>

int handle_exclusive_get(future* futureRef,int** valueRef)
{
		//kprintf("pid:%d, \n",futureRef->pid);
		
	if(futureRef->pid!=0)
	{
		return SYSERR;
	}

	if (futureRef->state == FUTURE_EMPTY) 
	{
		futureRef->state = FUTURE_WAITING;
		futureRef->pid = currpid;
		kprintf("suspend process %d\n",currpid);			
		suspend(currpid);		
		resched();
		**valueRef = futureRef->value;			
		return OK;
	}

	if(futureRef->state == FUTURE_VALID)
	{
		//futureRef->state = FUTURE_EMPTY;
		futureRef->pid = currpid;
		**valueRef = futureRef->value;	
		//kprintf("valid value: %d", futureRef->value);
		return OK;
	}
	
	if(futureRef->state == FUTURE_WAITING  )
	{	
		return SYSERR;
	}

	return OK;
}

int handle_shared_get(future* futureRef,int** valueRef)
{
	/*if (futureRef->state == FUTURE_EMPTY) 
	{
		futureRef->state = FUTURE_WAITING;
		futureRef->pid = currpid;
		kprintf("suspend process %d\n",currpid);	
		enq(&futureRef->get_queue, currpid);
		suspend(currpid);		
		resched();
		**valueRef = *futureRef->value;			
		return OK;
	}

	if(futureRef->state == FUTURE_VALID)
	{
		futureRef->state = FUTURE_EMPTY;
		futureRef->pid = 0;
		**valueRef = *futureRef->value;	
		return OK;
	}
	
	if(futureRef->state == FUTURE_WAITING  )
	{	
		return SYSERR;
	}
*/
	return OK;
}

int handle_queue_get(future* futureRef,int** valueRef)
{
	printf("3\n");
	return 0;
}

/*if a thread calls future_get() on future in FUTURE_EMPTY, then the calling thread should block and its thread id should get stored in the pid field of the future. Now subsequent calls to future_get() on a future should fail with SYSERR*/
syscall future_get(future* futureRef,int* valueRef){

	intmask mask;
	mask = disable();	
	int status;
	if(!futureRef){

		kprintf("get: future has already been used by another consumer and/or is NULL \n");
		restore(mask);
		return SYSERR;
	}
	
    	switch(futureRef->flag)
    	{
	    case FUTURE_EXCLUSIVE:
		status = handle_exclusive_get(futureRef, &valueRef);
		break;
	    case FUTURE_SHARED:
		status = handle_shared_get(futureRef, &valueRef);
		break;
	    case FUTURE_QUEUE:
		status = handle_queue_get(futureRef, &valueRef);
		break;
	    default:
		status = SYSERR;		
		break;
    	}
	
	restore(mask);
	return status;
}
