/* future_set.c - future_set */

#include <future.h>

syscall future_get(future* futureRef,int* valueRef){

	intmask mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/

	mask = disable();
	
	if (futureRef->state == FUTURE_VALID) {
		*valueRef = futureRef->value; 
		restore(mask);
		
		return OK;
	}

	if (futureRef->state == FUTURE_EMPTY) {		/* If caller must block	*/
		prptr = &proctab[currpid];
		prptr->prstate = PR_WAIT;	/* Set state to waiting	*/		
		futureRef->state = FUTURE_WAITING;
		futureRef->pid = currpid;
		resched();			/*   and reschedule	*/
	}
	
	*valueRef = futureRef->value; 
	restore(mask);
	return OK;
}