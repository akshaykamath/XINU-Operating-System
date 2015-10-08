/* future_set.c - future_set */

#include <future.h>

syscall future_set(future* futureRef,int* valueRef){
	intmask mask;			/* Saved interrupt mask		*/
	mask = disable();
	if(futureRef == NULL){
		restore(mask);
		return SYSERR;
	}
	int state = futureRef->state;
	
	pid32 pid = futureRef->pid;
	
	if(state == FUTURE_WAITING){
	   futureRef->value = *valueRef;
	   futureRef->state = FUTURE_VALID;
	   ready(pid);
	}
	restore(mask);
	return OK;	
}
