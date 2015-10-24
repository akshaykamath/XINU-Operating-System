#include<future.h>

int TestAndSet(future* futureRef) 
{
	if(futureRef == NULL)
	{
		return 0;
	}
	
	int oldval;
	oldval = futureRef->lock;    
	futureRef->lock = 1;
	
	return oldval;
}

void ReleaseLock(future* futureRef)
{
	futureRef->lock = 0;
	
}

