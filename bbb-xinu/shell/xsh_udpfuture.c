/* xsh_udpfuture.c - xsh_udpfuture */

//#include <xinu.h>

#include <future.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_udpfuture - does udp communication with linux server and synchronises all communication processes using futures.
 *------------------------------------------------------------------------
 */

uint32 net_prod(future *);
uint32 net_cons(future *);

shellcmd xsh_udpfuture(int nargs, char *args[]) {

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
	if (nargs > 1) 
	{
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		return 1;
	}
	
	future *f_exclusive;

	f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
	
	// Test NETWORK_FUTURE_EXCLUSIVE
        resume( create(net_cons, 1024, 20, "fcons1", 1, f_exclusive) );
	resume( create(net_prod, 1024, 20, "fprod1", 1, f_exclusive) );
	resume( create(net_cons, 1024, 20, "fcons1", 1, f_exclusive) );

	return 0;
}






