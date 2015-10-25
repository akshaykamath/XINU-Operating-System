#include<xinu.h>
#include<stdio.h>
#include<stdlib.h>
#include<fut_queue.h>

#ifndef _FUTURE_H_
#define _FUTURE_H_    

/* modes of operation for future*/
#define FUTURE_EXCLUSIVE  1	
#define FUTURE_SHARED     2	    
#define FUTURE_QUEUE      3     

/* define states */
#define FUTURE_EMPTY	  0
#define FUTURE_WAITING 	  1         
#define FUTURE_VALID 	  2         

/* modes of operation for future*/
#define FUTURE_EXCLUSIVE  1	
typedef struct futent
{
   int value;
   int flag;		
   int state;         	
   pid32 pid;
   sid32 lock;  
//   sid32 prodlock;
   int prodlock;
   queue *set_queue;     
   queue *get_queue;
} future;


/* Interface for system call */
future* future_alloc(int future_flags);
syscall future_free(future**);
syscall future_get(future*, int*);
syscall future_set(future*, int*);
int TestAndSet(future* );

/* producer declaration */
uint32 future_prod(future * fut);
uint32 future_cons(future * fut);
 
#endif /* _FUTURE_H_ */
