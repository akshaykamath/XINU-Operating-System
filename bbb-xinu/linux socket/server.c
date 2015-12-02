#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){
  int socketserver, nb;
  struct sockaddr_in server;
  int i;
  int j = 0;
  socklen_t addressSize;
  char buffer[2048];

  
  socketserver = socket(AF_INET, SOCK_DGRAM, 0);
  
  server.sin_family = AF_INET;
  server.sin_port = htons(5021);
  server.sin_addr.s_addr = inet_addr("192.168.1.100");
  memset(server.sin_zero, '\0', sizeof server.sin_zero);  
    
  bind(socketserver, (struct sockaddr *) &server, sizeof(server));  
  addressSize = sizeof server; 
 
  for(;;)
  {
	  nb = recvfrom(socketserver,buffer,1024,0,(struct sockaddr *)&server, &addressSize);
	  printf("Recd: %s\n", buffer);
	
	  for (i=0; i<10; i++) {
		    j += i;
	  }
	    
	  printf("Linux server produced: %d \n", j);
	  sprintf(buffer, "%d", j);   
	  sendto(socketserver, buffer, nb, 0, (struct sockaddr *)&server, sizeof(server));
  }  

  return 0;
}
