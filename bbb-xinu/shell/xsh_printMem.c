/* xsh_printmem.c - xsh_printmem */

#include <xinu.h>
#include <stdio.h>
#include <string.h>
/*------------------------------------------------------------------------
 * xsh_printmem - walks the list of free memory blocks and prints a line with the address and length of each block.
 *------------------------------------------------------------------------
 */

void test(int);
typedef struct charStr
{
   int a;
   int b;
   char c;
} charStr;

shellcmd xsh_printmem(int nargs, char *args[]) {

	/* Output info for '--help' argument */
	if (nargs == 2 ) 
	{
		if(strncmp(args[1], "--help", 7) == 0)
		{
			printf("Usage: %s\n\n", args[0]);
			printf("Description:\n");
			printf("\twalks the list of free memory blocks and prints a line with the address and length of each block.\n");
			printf("Options (one per invocation):\n");		
			printf("\t--help\tdisplay this help and exit\n");
			return 0;
		}
		else
		{
			fprintf(stderr, "%s: too many arguments\n", args[0]);
			return 1;
		}
	}

	/* Check argument count */
	/* If argument count is greater than 2, then there are too many arguments*/
	if (nargs > 2) 
	{
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		return 1;
	}
	
	
	intmask	mask;
	mask = disable();

	
	//Testing getpmem
	charStr * var = (struct charStr *)getpmem(sizeof(charStr));
	charStr * va2 = (struct charStr *)getmem(sizeof(charStr));

	resume( create(test, 1024, 20, "test1", 2, 2) );

	//kprintf("Total Free Memory : %u bytes \n",memlist.mlength);
	int i = 0;
	struct	memblk *curr;	/* Walk through memory list	*/
	curr = &memlist;
	//curr = curr->mnext;
	while (curr != NULL) {			/* Iterate free list	*/
		i++;
		kprintf("Address %u with %u bytes \n", curr, curr->mlength);
		curr = curr->mnext;
	}
	kprintf("%d chunks of memory blocks available \n", i);	
	restore(mask);
	return 0;
}

void test(int k){
	//kprintf("Hello from test\n");
	while(1);

}
