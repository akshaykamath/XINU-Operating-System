/* future_set.c - future_set */

#include <future.h>

syscall future_set(future* futureRef,int* valueRef){
	if(futureRef != NULL){
		int state = futureRef->state;
	}

	return SYSERR;	
}
