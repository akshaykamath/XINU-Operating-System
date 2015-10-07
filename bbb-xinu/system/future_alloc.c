/* future_alloc.c - future_alloc */

#include <future.h>

future* future_alloc(int future_flags){

	/*only support FUTURE_EXCLUSIVE mode*/
	if(future_flags != FUTURE_EXCLUSIVE){
		printf("Only FUTURE_EXCLUSIVE mode is supported");
		return NULL;
	}

	/*int occupies 8 bytes of memory for a 64-bit processor
	and int * occupies 8 bytes of memory
	So, struct future needs 48 bytes of memory*/

	char * memAddress = getmem(48);

	/* create a pointer to new future */
	future* futureVar = (future *)memAddress; // Verify at runtime issues -TODO
	futureVar->flag = future_flags; 
	futureVar->state = FUTURE_EMPTY;
	futureVar->value = 0;
	return futureVar;
}
