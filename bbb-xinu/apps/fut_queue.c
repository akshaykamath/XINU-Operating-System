#include<xinu.h>
#include <stdio.h>
#include <stdlib.h>
#include<fut_queue.h>

/*initialize queue with head and tail set to zero*/
int initqu(queue *qs)
{
    qs->tail = 0;
    qs->head = 0;
    return 0;
}

int enq(queue *qs, pid32 pid)
{
    // Overflow
    if(qs->tail == MAX_SIZE)
    {
        return -1;
    }

    qs->queue[qs->tail] = pid;
    qs->tail++;
    return 0;
}

pid32 deq(queue *qs)
{
    // Underflow: queue is empty.. reset
    if(qs->head == qs->tail)
    {
        qs->head = 0;
        qs->tail = 0;
        return SYSERR;
    }

    int pid = qs->queue[qs->head];    
    qs->head++;
    return pid ;
}

int peek(queue *qs)
{
	if(qs->head == qs->tail)
    {
        qs->head = 0;
        qs->tail = 0;
        return SYSERR;
    }

    int pid = qs->queue[qs->head];   
    return pid ;	
}

int isEmpty(queue *qs)
{
	if(qs->head == qs->tail)
{ 
	return 1;
}
return 0;
}
