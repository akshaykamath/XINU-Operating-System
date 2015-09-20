#include <prodcons.h>

void consumer(int count, sid32 consumed, sid32 produced)
{
	int i;
	for (i = 1; i <= count ; i++){
		wait(produced);
		printf("consumed: %d \n",n);
		signal(consumed);
	}
}
