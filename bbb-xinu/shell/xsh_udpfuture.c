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

int ssend()
{
 	int	i;			
	int	retval;			
	char	msg[] = "Test"; 
	char	replybuffer[1500];	
	int32	slot;			
	int32	msglen;			
	uint32	remoteip;		
	
	uint16	serverport= 5021;		
	uint16	localport = 5022;	
	int32	trials	= 100;		
	int32	delay	= 2000;		
	char	remoteipstr[] = "192.168.1.100";
	
	dot2ip(remoteipstr, &remoteip);

	slot = udp_register(remoteip, serverport, localport);
	if (slot == SYSERR) {
		fprintf(stderr, "%s: could not reserve UDP port %d\n",
				remoteipstr, localport);
		return 1;
	}

	msglen = strnlen(msg, 1200);
	for (i=0; i< trials; i++) {
		retval = udp_send(slot, msg, msglen);
		if (retval == SYSERR) {
			fprintf(stderr, "%s: error sending UDP \n",
				remoteipstr);
			return 1;
		}

		retval = udp_recv(slot, replybuffer, sizeof(replybuffer), delay);
		if (retval == TIMEOUT) 
		{
			fprintf(stderr, "%s: timeout...\n", remoteipstr);
			continue;
		} 
		else if (retval == SYSERR) 
		{
			fprintf(stderr, "%s: error from udp_recv \n", remoteipstr);
			udp_release(slot);
			return 1;
		}
		else
		{
			printf("network recv success.\n");
			int result = atoi(replybuffer);
			udp_release(slot);
			return result;
		}		
	}

	udp_release(slot);
	if (retval == TIMEOUT) {
		fprintf(stderr, "%s: retry limit exceeded \n",
			remoteipstr);
		return 1;
	}

	printf("UDP send test FAILED\n");
	return 1;
}

uint32 net_cons(future *fut) {
  int i, status;
  if(!fut){
   	intmask mask;
   	mask = disable();	
	kprintf("fcons : future is null\n");
	restore(mask);
	return SYSERR;
  }

  status = future_get(fut, &i);
  if (status == SYSERR) 
	{
	   intmask mask;
   	   mask = disable();
	 
	   kprintf("Process %d: future locked or non-existent\n", currpid);  
	   kprintf("Process %d: Exiting.\n", currpid); 
	   restore(mask);
	   return SYSERR;
	}


   intmask mask;
   mask = disable();
   kprintf("Process ID : %d consumed: %d\n",currpid, i);
  
   future_free(&fut);  

   restore(mask);
   return OK;
}

uint32 net_prod(future *fut) {
  int  j,status;

  if(!fut){
	intmask mask;
	mask = disable();
	kprintf("fprod : future is NULL\n");
	restore(mask);
	return SYSERR;
  }
  

   if(fut->flag == FUTURE_QUEUE)
   {
   	  intmask mask;
	  mask = disable();	
	  j = ssend();
	  
	  status = future_set(fut, &j);
	  kprintf("currpid :%d produced: %d, future set = %d\n",currpid, j, fut->value);
	  restore(mask);
   }
   else
   {
	j = ssend();
	  
	status = future_set(fut, &j);
  }	 

  if(status == SYSERR){
	intmask mask;
	mask = disable();
	kprintf("fprod :future is either null or empty state or already in valid state\n");
	restore(mask);
  }
  return OK;
}

