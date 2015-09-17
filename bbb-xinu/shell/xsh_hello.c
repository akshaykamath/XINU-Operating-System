/* xsh_hello.c - xsh_hello */

#include <xinu.h>
#include <stdio.h>
#include <string.h>
/*------------------------------------------------------------------------
 * xsh_hello - prints the welcome message
 *------------------------------------------------------------------------
 */
shellcmd xsh_hello(int nargs, char *args[]) {

	/* Output info for '--help' argument */
	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) 
	{
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays a welcome message for the given user name.\n");
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
	if (nargs < 2) 
	{
		fprintf(stderr, "%s: too few arguments\n", args[0]);
		return 1;
	}

	/* Check for invalid characters in the string. Display error message in the command. */
	int i = 0;
	for(i = 0; args[1][i] != '\0'; i++ )
	{
		if (!((args[1][i] >= 'a' && args[1][i]<= 'z') || (args[1][i] >= 'A' && args[1][i] <='Z' )))
		{			
			fprintf(stderr, "%s: Invalid characters in the string.\n", args[0]);
			return 1;
		}	
	}
	
	// Else, display the welcome message.
	printf("Hello %s, Welcome to the world of Xinu!!\n",args[1]);
	return 0;
}
