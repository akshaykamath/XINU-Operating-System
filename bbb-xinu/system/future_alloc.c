/* future_alloc.c - future_alloc */

#include <future.h>

future* future_alloc(int future_flags){

	/*only support FUTURE_EXCLUSIVE mode*/
	/*if(future_flags != FUTURE_EXCLUSIVE){
		intmask mask;
   		mask = disable();
		kprintf("Only FUTURE_EXCLUSIVE mode is supported\n");
		restore(mask);
		return NULL;
	}*/

	future* futureVar;
	futureVar = (struct future *)getmem(sizeof(future));
	futureVar->set_queue = (struct Queue *)getmem(sizeof(struct Queue));
	futureVar->get_queue = (struct Queue *)getmem(sizeof(struct Queue));
	initqu(&futureVar->get_queue);
	initqu(&futureVar->set_queue);
	futureVar->pid=0;
	futureVar->lock = 1;
	futureVar->prodlock = 0;
	
	
	if((int32)futureVar == SYSERR) {
		return NULL;
	}
	
	futureVar->flag = future_flags; 
	futureVar->state = FUTURE_EMPTY;
	futureVar->pid = 0;
	futureVar->value = 0;
	futureVar->lock = 0;
	return futureVar;
}
