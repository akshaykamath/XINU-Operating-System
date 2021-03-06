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

	
	// if both get and set queue are non empty
	if(isEmpty(&futureRef->get_queue) == 0 && isEmpty(&futureRef->set_queue) == 0)
	{
		// resume the set process
		pid32 setProcID = deq(&futureRef->set_queue);
		kprintf("set1: resuming producer %d\n", setProcID);
		resume(setProcID);
		// and just enque the new producer and wait for its turn to produce	
		enq(&futureRef->set_queue, currpid);
		kprintf("set1: suspending producer %d\n", currpid);
		suspend(currpid);		
		resched();
		kprintf("set1: resuming producer %d\n", currpid);
		// this producer is now resumed, just set the future and let the consumer know that the value is now set
		futureRef->value = *valueRef;

		// deque from get queue and resume the process
		pid32 procID = deq(&futureRef->get_queue);
		if(isEmpty(&futureRef->get_queue) == 0)
			futureRef->state = FUTURE_WAITING;
		else
			futureRef->state = FUTURE_VALID;

		futureRef->pid = procID;
		kprintf("set1: resuming consumer %d\n", procID);		
		resume(procID);		

		return OK;
	}

	// else if set queue is empty and there are consumers in the get queue, produce the value and resume the first consumer in queue
	if(isEmpty(&futureRef->set_queue) == 1 && isEmpty(&futureRef->get_queue) == 0)
	{

		futureRef->value = *valueRef;

		pid32 getProcID = deq(&futureRef->get_queue);		
		futureRef->pid = getProcID;
		
		if(isEmpty(&futureRef->get_queue) == 0)
			futureRef->state = FUTURE_WAITING;
		else
			futureRef->state = FUTURE_VALID;
		resume(getProcID);
		
		return OK;		
	}

	// else: there is nothing in the get queue and nothing in the set queue, just enqueue the producer and wait for resumption
	enq(&futureRef->set_queue, currpid);
	kprintf("set3: suspending producer %d\n", currpid);
	suspend(currpid);		
	resched();
	kprintf("set3: resuming producer %d\n", currpid);
	// this producer is now resumed, just set the future and let the consumer know that the value is now set
	futureRef->value = *valueRef;
	

	
	// deque from get queue and resume the process
	pid32 procID = deq(&futureRef->get_queue);
	if(isEmpty(&futureRef->get_queue) == 0)
		futureRef->state = FUTURE_WAITING;
	else
		futureRef->state = FUTURE_VALID;

	futureRef->pid = procID;		
	kprintf("set2: resuming producer %d \n", procID);
	resume(procID);		

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
