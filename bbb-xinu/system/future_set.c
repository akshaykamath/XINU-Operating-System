/* future_set.c - future_set */

#include <future.h>


int handle_exclusive_set(future* futureRef,int* valueRef)
{
	if(futureRef->state == FUTURE_VALID )
	{
		return SYSERR;
	}
	
	if(futureRef->state == FUTURE_EMPTY)
	{
		futureRef->value= *valueRef;		
		futureRef->state= FUTURE_VALID;			
	}

	else if(futureRef->state == FUTURE_WAITING)
	{		
		futureRef->value = *valueRef;		
		futureRef->state= FUTURE_VALID;	
		/*resume only one consumer that is waiting for producer, no queue is used here*/		
		resume(futureRef->pid);
		resched();		
	}
	
	return OK;
}

int handle_shared_set(future* futureRef,int* valueRef, intmask mask)
{

	if(futureRef->state == FUTURE_VALID)
	{
		restore(mask);
		//since only one producer can call future set, it has already set it to waiting,all other producers should throw error
		return SYSERR;
	}else if(futureRef->state == FUTURE_EMPTY)
	{
		futureRef->state = FUTURE_VALID;
		futureRef->value = *valueRef;
		restore(mask);
		return OK;
	}
	else if(futureRef->state == FUTURE_WAITING)
	{		
		futureRef->value = *valueRef;		
		futureRef->state= FUTURE_VALID;			
		pid32 procID = deq(&futureRef->get_queue);
		futureRef->pid = procID;
		//kprintf("proq: %d", procID);
		restore(mask);
		resume(procID);		
	}

	return OK;
}

int handle_queue_set(future* futureRef,int* valueRef)
{
	/*Case 1- If a thread is calling future_set and there are threads waiting for value in get_queue then thread calling future_set should set the value and resume only one thread from get_queue based 		on first come first serve basis. If there is no thread waiting in get_queue then thread calling future_set should enqueue itself in set_queue.*/

	//kprintf("set - future state %d : \n",futureRef->state);
	//kprintf("set - Saved Pid flag %d: \n",futureRef->pid);
	
	if(futureRef->state == FUTURE_EMPTY)
	{
		//This is the case when there is no thread waiting for future
		futureRef->value = *valueRef;	
			
		//kprintf("Suspending producer process %d\n",currpid);	
		enq(&futureRef->set_queue, currpid);
		//suspend(currpid);		
		resched();	
		return OK;
	}else if(futureRef->state == FUTURE_WAITING)
	{
		
		futureRef->value = *valueRef;
		futureRef->state = FUTURE_VALID;
		if(isEmpty(&futureRef->get_queue) == 0){
			pid32 waitingProcID = deq(&futureRef->get_queue);
			//kprintf("Resuming consumer process %d : \n",waitingProcID);
			//There is atleast one process waiting for this set.I assume that this will always be true when state is WAITING
			resume(waitingProcID);
		}
		else{
			futureRef->value = *valueRef;	
			enq(&futureRef->set_queue, currpid);	
			resched();	
		}
		return OK;
			
	}else if(futureRef->state == FUTURE_VALID){
	
		return OK;
	
	}
	return OK;
}


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
		status = handle_exclusive_set(futureRef, valueRef);
		break;
	    case FUTURE_SHARED:
		status = handle_shared_set(futureRef, valueRef, mask);
		break;
	    case FUTURE_QUEUE:
		status = handle_queue_set(futureRef, valueRef);
		break;
	    default:
		status = SYSERR;
		break;
    	}
	
	restore(mask);
	return status;
}
