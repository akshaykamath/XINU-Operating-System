/* future_set.c - future_set */

#include <future.h>

syscall future_free(future* futureRef){

	return freemem((char *)futureRef, sizeof(future));;	
}