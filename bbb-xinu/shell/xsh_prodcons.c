#include <prodcons.h>
#include <ctype.h>
#include <stdlib.h>

int n ;                 //Definition for global variable 'n'
sid32 consumed,produced;

shellcmd xsh_prodcons(int nargs, char *args[])
{
        //Argument verifications and validations

        int count = 2000;             //local variable to hold count
	int i = 0;
	int useFutures = 0;
	
	// Initialise the value of n to 0, since this is an extern variable, it may start with the previous value
	
 	/* Output info for '--help' argument */
	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) 
	{
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tProducer Consumer solution using semaphore synchronization or futures.\n");
		printf("Options (one per invocation):\n");		
		printf("\t--help\tdisplay this help and exit\n");
		printf("\t-f\tUses futures and promises\n");
		printf("\tn\tnumber of values to be produced and consumed,default is 2000\n");
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
		if(strcmp(args[1],"-f")==0){
			useFutures = 1;
		}
		else
		{
			// Parse through the array of parameters and return 1 if there is a character other than a number.
			for(i = 0; args[1][i] != '\0'; i++ )
			{
				if (isdigit(args[1][i]) == 0)
				{
					fprintf(stderr, "%s: input parameter should be an integer.\n", args[0]);
					return 1;	
				}
			}
		
			// Else, it can be safely converted to a number.
			count =  atoi(args[1]);	
		}
	}
	
	if(useFutures)
	{
	      future * f1, *f2, *f3  ;
	      f1 = future_alloc(FUTURE_EXCLUSIVE);	      
	      f2 = future_alloc(FUTURE_EXCLUSIVE);
	      f3 = future_alloc(FUTURE_EXCLUSIVE);
	      if(f1 == NULL || f2 == NULL || f3 == NULL)
	      {
		return 1;
	      }	

 			       
		resume( create(future_prod, 1024, 20, "fprod12", 1, f1) );
		resume( create(future_prod, 1024, 20, "fprod13", 1, f1) );
		resume( create(future_cons, 1024, 20, "fcons11", 1, f1) ); 	       
		resume( create(future_cons, 1024, 20, "fcons11", 1, f1) );
		resume( create(future_cons, 1024, 20, "fcons11", 1, f1) ); 	        	       
		//resume( create(future_cons, 1024, 20, "fcons12", 1, f1) ); 
		// After Usage is completed delete the future
		//freefutures(&f1);


 		resume( create(future_cons, 1024, 20, "fcons21", 1, f2) ); 	        
resume( create(future_cons, 1024, 20, "fcons22", 1, f2) ); 	        
		resume( create(future_prod, 1024, 20, "fprod22", 1, f2) );
		//freefutures(&f2);
	      	
 	      	resume( create(future_cons, 1024, 20, "fcons31", 1, f3) );
	      	resume( create(future_prod, 1024, 20, "fprod32", 1, f3) );
		//freefutures(&f3);

	}
	else
	{
		if(count == 0){
			fprintf(stderr, "Count should be greater than zero.\n");
			return 1;
		}
	      consumed = semcreate(1);
	      produced = semcreate(0);
	      resume( create(consumer, 1024, 20, "cons", 3, count, consumed, produced) );
	      resume( create(producer, 1024, 20, "prod", 3, count, consumed, produced) );
	} 

      return 0;
}

void freefutures(future **f1)
{
	while (TestAndSet(*f1));
	future_free(f1);	
}




