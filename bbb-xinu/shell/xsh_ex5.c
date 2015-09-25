/* xsh_hello.c - xsh_ex5 */

#include <xinu.h>
#include <stdio.h>
#include <string.h>
/*------------------------------------------------------------------------
 * xsh_hello - prints the welcome message
 *------------------------------------------------------------------------
 */

void prod2(sid32, sid32), cons2(sid32, sid32);

int buffer[15];
shellcmd xsh_ex5(int nargs, char *args[]) {

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
	
	/* If argument count is less than 2, then there are too few arguments*/
	if (nargs > 1) 
	{
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		return 1;
	}

	sid32 produced, consumed;
	consumed = semcreate(0);
	produced = semcreate(1);
	resume( create(cons2, 1024, 20, "cons", 2, consumed, produced) );
	resume( create(prod2, 1024, 20, "prod", 2, consumed, produced) );	
	
	return 0;
}

/*------------------------------------------------------------------------
* prod2 -- increment n 2000 times, waiting for it to be consumed
*------------------------------------------------------------------------
*/
void prod2(
sid32 consumed,
sid32 produced
)
{
int32 i;

int j;
for( i=1 ; i<=2000 ; i++ ) {

wait(consumed);

for (j = 1; j <= 15; j++)
{
	buffer[j] = i;
	printf("produced: %d \n", buffer[j]);		
}

signal(produced);
}
}
/*------------------------------------------------------------------------
* cons2 -- print n 2000 times, waiting for it to be produced
*------------------------------------------------------------------------
*/
void cons2(
sid32 consumed,
sid32 produced
)
{
int i;

wait(produced);
for(i = 1; i<=15;i++)
{
	printf("consumed: %d \n", buffer[i]);
}

signal(consumed);

}
