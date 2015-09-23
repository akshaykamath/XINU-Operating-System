#include <prodcons.h>

void consumer(int count)
{
	/*int i;
	for (i = 1; i <= count ; i++){
		printf("consumed: %d \n",n);
	}*/
	
	while (1){
		printf("consumed: %d \n",n);
		if ( n == count){
			break;
		}
	}
}
