/* future_alloc.c - future_alloc */

#include <future.h>

future* future_alloc(int future_flags){

	/*only support FUTURE_EXCLUSIVE mode*/
	if(future_flags != FUTURE_EXCLUSIVE){
		printf("Only FUTURE_EXCLUSIVE mode is supported");
		return NULL;
	}

	future* futureVar;
	futureVar = (struct future *)getmem(28);

	if((int32)futureVar == SYSERR) {
		return NULL;
	}
	
	futureVar->flag = future_flags; 
	futureVar->state = FUTURE_EMPTY;
	futureVar->value = 0;
	return futureVar;
}
