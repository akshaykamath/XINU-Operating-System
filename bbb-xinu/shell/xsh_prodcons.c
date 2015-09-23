#include <prodcons.h>

int n ;                 //Definition for global variable 'n'
/*Now global variable n will be on Heap so it is accessible all the processes i.e. consume and produce*/

shellcmd xsh_prodcons(int nargs, char *args[])
{
      //Argument verifications and validations

      int count = 2000;             //local varible to hold count
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

	/* If argument count is equal to 2, then assign args[1] to count variable */
	if (nargs == 2) 
	{
		count =  atoi(args[1]);
	}
	
      //create the process producer and consumer and put them in ready queue.
      //Look at the definitions of function create and resume in exinu/system folder for reference.  
	
      resume( create(producer, 1024, 20, "producer", 3, count) );
      resume( create(consumer, 1024, 20, "consumer", 3, count) );
      
	
}
