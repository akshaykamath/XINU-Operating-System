#ifndef _FUT_QUEUE_H_
#define _FUT_QUEUE_H_

#define MAX_SIZE 10

typedef struct Queue
{

int tail;
int queue[MAX_SIZE];
int head;
}queue;

int initqu(queue *qs);
int enq(queue *qs, int pid);
int deq(queue *qs);
int peek(queue *qs);
int isEmpty(queue *qs);
#endif /* _FUTURE_H_ */
