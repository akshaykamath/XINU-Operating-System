#include <prodcons.h>

void consumer(int count)
{	
	while (1){

		printf("consumed: %d \n",n);		
		if ( n == count){
			break;
		}
	}
}
