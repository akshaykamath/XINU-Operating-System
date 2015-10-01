#include <prodcons.h>

 void producer(int count, sid32 consumed, sid32 produced)
 {
      	//Code to produce values less than equal to count, 
	int i;	
	for(i = 1; i <= count; i++)
	{
		wait(consumed);
		n = i;
		printf("produced: %d \n",n);
		signal(produced);
	}

	// Delete the consumed semaphore once the produced semaphore after completion.
	semdelete(produced);
 }


