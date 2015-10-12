
#include<future.h>

//lock = 0;
int TestAndSet(future* futureRef) 
{
	intmask mask;
	mask = disable();
	int oldval;
	oldval = futureRef->lock;    
	futureRef->lock = 1;
	restore(mask);
	return oldval;
}

void ReleaseLock(future* futureRef)
{
	futureRef->lock = 0;
	
}

