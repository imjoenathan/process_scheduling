/* ready.c - ready */

#include <xinu.h>

qid16	readylist[10];			/* Index of ready list		*/


/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{

	int debug = 0;
	register struct procent *prptr;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;
	prptr->pr_tsready = clktime;
/*	insert(pid, readylist, prptr->prprio);		*/
if (debug){
	kprintf("before enqueue: %d\n", firstid(readylist[prptr->prprio]));
}
	int check = enqueue(pid, readylist[prptr->prprio]);
if (debug) {
	kprintf("after enqueue: %d\n", firstid(readylist[prptr->prprio]));
	kprintf("here is the return value of enqueue(): %d\n\n", check);
	kprintf("____________READY_____________\n");
	kprintf("Name: %s\n", prptr->prname);
	kprintf("Priority: %d\n", prptr->prprio);
	kprintf("______________________________\n\n");
}
	resched();

	return OK;
}
