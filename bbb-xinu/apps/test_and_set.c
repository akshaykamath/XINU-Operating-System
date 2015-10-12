
#include<future.h>

lock = 0;

int TestAndSet() 
{
	intmask mask;
	mask = disable();
	int oldval;
	oldval = lock;    
	lock = 1;
	restore(mask);
	return oldval;
}

void ReleaseLock()
{
	lock = 0;
}

