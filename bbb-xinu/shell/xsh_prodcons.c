#include <prodcons.h>

int n ;                 //Definition for global variable 'n'
/*Now global variable n will be on Heap so it is accessible all the processes i.e. consume and produce*/

shellcmd xsh_prodcons(int nargs, char *args[])
{
      //Argument verifications and validations

      int count = 2000;             //local varible to hold count
    	//check args[1] if present assign value to count
 	/* Output info for '--help' argument */
	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) 
	{
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tProducer Consumer Example.\n");
		printf("Options (one per invocation):\n");		
		printf("\t--help\tdisplay this help and exit\n");
		return 0;
	}

	/* Check argument count */
	/* If argument count is greater than 2, then there are too many arguments*/
	if (nargs > 2) 
	{
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		return 1;
	}

	/* If argument count is less than 2, then there are too few arguments*/
	if (nargs == 2) 
	{
		//To do for more than one digit count
		count =  atoi(args[1]);
	}
	
      //create the process producer and consumer and put them in ready queue.
      //Look at the definitions of function create and resume in exinu/system folder for reference.  
	sid32 produced,consumed;
	consumed = semcreate(0);
	produced = semcreate(1);    
      resume( create(producer, 1024, 20, "producer", 3, count, consumed, produced) );
      resume( create(consumer, 1024, 20, "consumer", 3, count, consumed, produced) );
	printf("Count is %d", count);
	
}
