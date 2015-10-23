/* future_set.c - future_set */

#include <future.h>


int handle_exclusive_set(future* futureRef,int* valueRef, intmask mask)
{
	if(futureRef->state == FUTURE_VALID )
	{
		return SYSERR;
	}
	
	if(futureRef->state == FUTURE_EMPTY)
	{
		futureRef->value= *valueRef;		
		futureRef->state= FUTURE_VALID;		

		//kprintf("resume process %d\n",futureRef->value);
		restore(mask);	
		return OK;	
	}

	else if(futureRef->state == FUTURE_WAITING)
	{		
		futureRef->value = *valueRef;		
		futureRef->state= FUTURE_VALID;			
		
		resume(futureRef->pid);
		resched();		
		return OK;
	}

	return OK;
}

int handle_shared_set(future* futureRef,int* valueRef)
{
/*
kprintf("here");
	if(futureRef->state == FUTURE_VALID || futureRef->state == FUTURE_EMPTY)
	{
		return SYSERR;
	}
	else if(futureRef->state == FUTURE_WAITING)
	{		
		futureRef->value = valueRef;		
		futureRef->state= FUTURE_VALID;	
		int resProc = deq(&futureRef->get_queue);
		kprintf("resume process %d\n",resProc);	
		resume(resProc);
		resched();
		
		return OK;
	}
*/
	return OK;
}

int handle_queue_set(future* futureRef,int* valueRef)
{
	printf("3\n");
	return 0;
}

/* If a thread calls future_set() on a future in the FUTURE_EMPTY state, then the value provided by  the future_set() call should get stored in the value field of the future and its state should change from FUTURE_WAITING to FUTURE_VALID. Then subsequent calls to future_set() for the same future should fail.
*/
syscall future_set(future* futureRef,int* valueRef){

	intmask mask;
	mask = disable();	
	int status;
	if(!futureRef){
		kprintf("set: future is NULL\n");
		restore(mask);
		return SYSERR;
	}
	
    	switch(futureRef->flag)
    	{
	    case FUTURE_EXCLUSIVE:
		status = handle_exclusive_set(futureRef, valueRef, mask);
		break;
	    case FUTURE_SHARED:
		status = handle_shared_set(futureRef, valueRef);
		break;
	    case FUTURE_QUEUE:
		status = handle_queue_set(futureRef, valueRef);
		break;
	    default:
		kprintf("Return set %d\n", futureRef->flag);
		break;
    	}
//kprintf("endset\n");	
	restore(mask);
	return status;
}
