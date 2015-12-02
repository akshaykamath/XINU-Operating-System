/* udp.c - udp_init, udp_in, udp_register, udp_send, udp_sendto,	*/
/*	        udp_recv, udp_recvaddr, udp_release, udp_ntoh, udp_hton	*/

#include <xinu.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int udpsend()
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
