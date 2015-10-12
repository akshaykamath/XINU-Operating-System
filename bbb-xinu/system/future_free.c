/* future_set.c - future_set */

#include <future.h>

syscall future_free(future** futureRef){

	int status;
	status = freemem((struct future *)(*futureRef), sizeof(future));
	if(status == OK){
		printf("Memory freed successfully\n");
		*futureRef = NULL;
		return OK;
	}
	printf("free \n");
	printf("%d\n",status);

	return status;	
}
