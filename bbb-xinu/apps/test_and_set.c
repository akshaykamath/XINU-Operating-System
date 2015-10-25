#include<future.h>

int TestAndSet(future* futureRef) 
{
	if(futureRef == NULL)
	{
		return 0;
	}
	
	int oldval;
	//oldval = futureRef->prodlock;    
	//futureRef->prodlock = 1;
	
	return oldval;
}

void ReleaseLock(future* futureRef)
{
	//futureRef->prodlock = 0;
	
}

