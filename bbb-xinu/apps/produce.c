#include <prodcons.h>

 void producer(int count, sid32 consumed, sid32 produced)
 {
      //Code to produce values less than equal to count, 
	int i;
	n = 0;
	for(i = 1; i <= count; i++)
	{
		wait(consumed);
		n++;
		printf("produced: %d \n",n);
		signal(produced);
	}
 }


