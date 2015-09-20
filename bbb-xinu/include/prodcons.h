#include <xinu.h> 
#include <stddef.h>
 #include <stdio.h>   

/*Global variable for producer consumer*/
extern int n; /*this is just declaration*/

/*function Prototype*/
void consumer(int count, sid32 consumed, sid32 produced);
void producer(int count, sid32 consumed, sid32 produced);
