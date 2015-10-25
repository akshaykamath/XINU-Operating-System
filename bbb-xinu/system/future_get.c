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
		kprintf("suspending consumer %d\n",currpid);			
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

	if (futureRef->state == FUTURE_EMPTY || futureRef->state == FUTURE_WAITING || (futureRef->state == FUTURE_VALID && futureRef->pid != 0)) 
	{
		futureRef->state = FUTURE_WAITING;

		kprintf("suspending consumer %d\n",currpid);	
		enq(&futureRef->get_queue, currpid);
		suspend(currpid);		
		resched();
		futureRef->pid = 0;
		kprintf("Resuming consumer: %d\n",currpid);
		**valueRef = futureRef->value;			
		return OK;
	}

	if(futureRef->state == FUTURE_VALID)
	{
		
		**valueRef = futureRef->value;
		return OK;
	}

	return OK;
}

int handle_queue_get(future* futureRef,int** valueRef)
{
	/*If a thread is calling future_get and there are threads waiting to set value in set_queue then thread should release only one thread from set_queue and should get the value set by thread just 		  released from set_queue. If there is no thread waiting in set_queue to set the value then thread calling future_get should enqueue itself in get_queue.*/

	// if only set queue is not empty
	if(isEmpty(&futureRef->set_queue) == 0 && isEmpty(&futureRef->get_queue) == 1)
	{
		// resume the set process
		pid32 setProcID = deq(&futureRef->set_queue);	
		futureRef->pid = currpid;		
		kprintf("get1: resuming producer %d\n", setProcID);

		resume(setProcID);	
		futureRef->state = FUTURE_WAITING;
		
		**valueRef = futureRef->value;	

		if(isEmpty(&futureRef->get_queue) == 0)
			futureRef->state = FUTURE_WAITING;
		else
			futureRef->state = FUTURE_EMPTY;

		//futureRef->pid = 0;

		return OK;
	}

	// else if both the queues have values
	if(isEmpty(&futureRef->set_queue) == 0 && isEmpty(&futureRef->get_queue) == 0)
	{
		pid32 setProcID = deq(&futureRef->set_queue);		
		
		resume(setProcID);	
		enq(&futureRef->get_queue, currpid);
		futureRef->state = FUTURE_WAITING;
		futureRef->pid = currpid;
		kprintf("get2: suspending consumer %d\n", currpid);
		suspend(currpid);		
		resched();
		kprintf("get2: resuming producer %d\n", setProcID);
		
		if(isEmpty(futureRef->get_queue) == 0)
			futureRef->state = FUTURE_WAITING;
		else
			futureRef->state = FUTURE_EMPTY;
		**valueRef = futureRef->value;			
		//futureRef->pid = 0;
			
		return OK;		
	}
	
	// if none of the above cases apply, just enqueue the current process (there is no producer in queue to produce, just wait)

	enq(&futureRef->get_queue, currpid);	

	futureRef->state = FUTURE_WAITING;
	futureRef->pid = currpid;
	kprintf("get3: suspending consumer %d\n", currpid);
	suspend(currpid);		
	resched();

	kprintf("get3: resuming consumer %d\n", currpid);
	if(isEmpty(&futureRef->get_queue) == 1){
		futureRef->state = FUTURE_WAITING;	
	}
	else{
		futureRef->state = FUTURE_EMPTY;futureRef->pid = 0;
	}
	**valueRef = futureRef->value;	
	//futureRef->pid = 0;
	
	return OK;		
}


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
		//kprintf("Calling exclusive get for consumer process id %d\n",currpid);
		status = handle_exclusive_get(futureRef, &valueRef);
		break;
	    case FUTURE_SHARED:
		//kprintf("Calling shared get for consumer process id %d\n",currpid);
		status = handle_shared_get(futureRef, &valueRef);
		break;
	    case FUTURE_QUEUE:
		//kprintf("Calling queue get for consumer process id %d\n",currpid);
		status = handle_queue_get(futureRef, &valueRef);
		break;
	    default:
		status = SYSERR;		
		break;
    	}
	
	restore(mask);
	return status;
}
