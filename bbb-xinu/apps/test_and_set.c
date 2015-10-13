#include<future.h>

int TestAndSet(future* futureRef) 
{
	if(futureRef == NULL)
	{
		return 0;
	}
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

