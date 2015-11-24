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
	//printf("Hello %s, Welcome to the world of Xinu!!\n",args[1]);
	return ssend();

//	return 0;
}

int ssend()
{
 	int	i;			/* index into buffer		*/
	int	retval;			/* return value			*/
	char	msg[] = "Xinu testing UDP echo"; /* message to send	*/
	char	inbuf[1500];		/* buffer for incoming reply	*/
	int32	slot;			/* UDP slot to use		*/
	int32	msglen;			/* length of outgoing message	*/
	uint32	remoteip;		/* remote IP address to use	*/
	//uint32	localip;		/* local IP address to use	*/
	uint16	echoport= 7891;		/* port number for UDP echo	*/
	uint16	locport	= 7892;	/* local port to use		*/
	int32	retries	= 3;		/* number of retries		*/
	int32	delay	= 2000;		/* reception delay in ms	*/
	char	remoteipstr[] = "192.168.1.100";

	/* register local UDP port */
	dot2ip(remoteipstr, &remoteip);
	slot = udp_register(remoteip, echoport, locport);
	if (slot == SYSERR) {
		fprintf(stderr, "%s: could not reserve UDP port %d\n",
				remoteipstr, locport);
		return 1;
	}

	/* Retry sending outgoing datagram and getting response */

	msglen = strnlen(msg, 1200);
	for (i=0; i<retries; i++) {
		retval = udp_send(slot, msg, msglen);
		if (retval == SYSERR) {
			fprintf(stderr, "%s: error sending UDP \n",
				remoteipstr);
			return 1;
		}

		retval = udp_recv(slot, inbuf, sizeof(inbuf), delay);
		if (retval == TIMEOUT) {
			fprintf(stderr, "%s: timeout...\n", remoteipstr);
			continue;
		} else if (retval == SYSERR) {
			fprintf(stderr, "%s: error from udp_recv \n",
				remoteipstr);
			udp_release(slot);
			return 1;
		}
		break;
	}

	udp_release(slot);
	if (retval == TIMEOUT) {
		fprintf(stderr, "%s: retry limit exceeded\n",
			remoteipstr);
		return 1;
	}

	/* Response received - check contents */

	if (retval != msglen) {
		fprintf(stderr, "%s: sent %d bytes and received %d\n",
			remoteipstr, msglen, retval);
		return 1;
	}
	for (i = 0; i < msglen; i++) {
		if (msg[i] != inbuf[i]) {
			fprintf(stderr, "%s: reply differs at byte %d\n",
				remoteipstr, i);
			fprintf(stderr, "string recd: %s\n", inbuf);
			return 1;
		}
	}

	

	printf("UDP send test was successful\n");
	return 0;
}

int rrecv()
{
}


