/* future_set.c - future_set */

#include <future.h>

/*if a thread calls future_get() on future in FUTURE_EMPTY, then the calling thread should block and its thread id should get stored in the pid field of the future. Now subsequent calls to future_get() on a future should fail with SYSERR*/
syscall future_get(future* futureRef,int* valueRef){

	intmask mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/

	mask = disable();
	
	if (futureRef->state == FUTURE_VALID) {
		*valueRef = futureRef->value; 
		futureRef->state = FUTURE_EMPTY;
		printf("get:st V v %d \n",*valueRef);
		printf("get:st E\n");
		restore(mask);
		return OK;
	}
	if(futureRef->state == FUTURE_WAITING){
		printf("get:W error\n");
		restore(mask);
		return SYSERR;
	}

	if (futureRef->state == FUTURE_EMPTY) {		/* If caller must block	*/
		//prptr = &proctab[currpid];
		//prptr->prstate = PR_WAIT;	/* Set state to waiting	*/		
		futureRef->state = FUTURE_WAITING;
		futureRef->pid = currpid;
		//test code starts here
		printf("get:E W\n");
		printf("while starts \n");
		/*while(1){
			if(futureRef->state == FUTURE_VALID){	
				*valueRef = futureRef->value;
				futureRef->state = FUTURE_EMPTY;
				//printf("get:V v %d \n",*valueRef);
				break;
			}
		}*/
		printf("While ends \n");
		//test code ends here
	}
	
	
	restore(mask);
	return OK;
}
