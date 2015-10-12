
#include<future.h>

lock = 0;

int TestAndSet() 
{
	int oldval;
	oldval = lock;    
	lock = 1;
	return oldval;
}

void ReleaseLock()
{
	lock = 0;
}

