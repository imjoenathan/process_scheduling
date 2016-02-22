/* yield.c - yield */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  yield  -  Voluntarily relinquish the CPU (end a timeslice)
 *------------------------------------------------------------------------
 */
syscall	yield(void)
{
	intmask	mask;			/* Saved interrupt mask		*/

	mask = disable();

	/* Set yeilded property to 1 for process */
	
	proctab[currpid].pr_yielded = 1; 

	resched();
	restore(mask);
	return OK;
}
