/* kill.c - kill */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/
	int32	i;			/* Index into descriptors	*/

	mask = disable();
	
	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE) {
		restore(mask);
		return SYSERR;
	}

	if (--prcount <= 1) {		/* Last user process completes	*/
		xdone();
	}

	send(prptr->prparent, pid);
	for (i=0; i<3; i++) {
		close(prptr->prdesc[i]);
	}

	if(simpleAlloc != currpid)
	{
		kprintf("free stack\n");
		freestk(prptr->prstkbase, prptr->prstklen);
	}
	
	simpleAlloc = 0;

	// Stack space consumptions starts
	uint32 ssize = prptr->prstklen;
	char * startAddr = (char *)prptr->prstkbase;
	kprintf("Stack size %d bytes\n",ssize);
	int cnt=1;
	int cntRemaining = 0;
	
	while(1)
	{
		if(cnt == ssize)
		{
			break;
		}

		if(*startAddr == 'f')
		{
		  cntRemaining ++;
		}
		
		startAddr--;
		cnt++;
	}
	
	int unused = ssize - cntRemaining;
	kprintf("Stack space used %u bytes\n", unused );
	kprintf("Stack space unused %u bytes\n", cntRemaining);
	//kprintf("Stack address %u \n", startAddr);

	// Stack space consumptions ends

	//freestk(prptr->prstkbase, prptr->prstklen);

	switch (prptr->prstate) {
	case PR_CURR:
		prptr->prstate = PR_FREE;	/* Suicide */
		resched();

	case PR_SLEEP:
	case PR_RECTIM:
		unsleep(pid);
		prptr->prstate = PR_FREE;
		break;

	case PR_WAIT:
		semtab[prptr->prsem].scount++;
		/* Fall through */

	case PR_READY:
		getitem(pid);		/* Remove from queue */
		/* Fall through */

	default:
		prptr->prstate = PR_FREE;
	}

	restore(mask);
	return OK;
}
