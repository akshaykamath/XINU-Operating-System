#include <prodcons.h>

void consumer(int count, sid32 consumed, sid32 produced)
{	
	
	while (1){
		
		wait(produced);
		printf("consumed: %d \n",n);		
		if ( n == count){
			break;
		}
		signal(consumed);
	}
}
