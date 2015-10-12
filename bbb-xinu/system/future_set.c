/* future_set.c - future_set */

#include <future.h>

/* If a thread calls future_set() on a future in the FUTURE_EMPTY state, then the value provided by  the future_set() call should get stored in the value field of the future and its state should change from FUTURE_WAITING to FUTURE_VALID. Then subsequent calls to future_set() for the same future should fail.
*/
syscall future_set(future* futureRef,int* valueRef){

	if(!futureRef){
		printf("set: future is NULL\n");
		return SYSERR;
	}
	
	if(futureRef->state == FUTURE_VALID){
		return SYSERR;
	}
	else if(futureRef->state == FUTURE_EMPTY || futureRef->state == FUTURE_WAITING){
		futureRef->value = *valueRef;		
		futureRef->state= FUTURE_VALID;		
	}
	return OK;	
}
