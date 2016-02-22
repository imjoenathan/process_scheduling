/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;
pid32	dequeue(qid16);

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	int debug=0;

	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];

	/* Get class of ptold as I/O or CPU Bound before reclassifying */

	bool8 origClass = ptold->pr_class;

	/* Reclassify the ptold as I/O or CPU Bound */
 
	if (!ptold->pr_yielded && ptold->prstate == PR_CURR && preempt > 0) {
		ptold->pr_class = PRCLS_CPUB;
	}

	if (ptold->prstate == PR_RECV || ptold->prstate == PR_SLEEP ||  
	    ptold->prstate == PR_SUSP || ptold->prstate == PR_WAIT ||
	    ptold->prstate == PR_RECTIM) {

		ptold->pr_class = PRCLS_IOB;
	}

	if (ptold->prstate == PR_CURR && ptold->pr_yielded) {
		ptold->pr_class = PRCLS_IOB;
	}


	/* Adjust priority of ptold here 	*/
	if (ptold->prprio == 9) {
		//Do Nothing
	}
	else 	if (origClass == 0 && 
		    ptold->pr_class == PRCLS_CPUB){
			
			ptold->prprio--;
			
			if (ptold->prprio < 0) {
				ptold->prprio = 0;
			}
		}
	else	if (origClass == 1 &&
		    ptold->pr_class == PRCLS_IOB) {

			ptold->prprio++;

			if (ptold->prprio > 8) {
				ptold->prprio = 8;
			}
		}
	else	if (origClass == 0 &&
		    ptold->pr_class == PRCLS_IOB) {

			ptold->prprio += 2;

			if (ptold->prprio > 8) {
				ptold->prprio = 8;
			}
		}
	else	if (origClass == 1 &&
		    ptold->pr_class == PRCLS_CPUB) {

			ptold->prprio -= 2;

			if (ptold->prprio < 0) {
				ptold->prprio = 0;
			}
		}

	


	/* Check here what the highest ready process priority is	*/
if (debug) {
	kprintf("_____OLD PROCESS AFTER PRIO ADJUST_______\n");
	kprintf("Name: %s\n", ptold->prname);
	kprintf("Priority: %d\n", ptold->prprio);
	kprintf("_________________________________________\n\n");
}

	int32 top;
	for (top = 9; top >= 0; --top) {
		if (!isempty(readylist[top])) {
			break;
		}
	}
	if (top < 0) {
		top = 0;
		
	}

	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		if (ptold->prprio > top) {
			return;
		}

		/* Old process will no longer remain current */

				
		ptold->prstate = PR_READY;

		pid32 ch;
		ch = enqueue(currpid, readylist[ptold->prprio]);
	
	}

	/* Force context switch to highest priority ready process */
	/* TODO: CHECK ALL READYLIST QUEUES FOR HIGHEST PRIORITY PROCESS	*/
	
	currpid = dequeue(readylist[top]);

	
	ptnew = &proctab[currpid];

	ptnew->prstate = PR_CURR;

	/* Associate a new quantum time		*/
	switch (ptnew->prprio) {

		case 0:
			preempt = 200;
			break;

		case 1:
			preempt = 180;
			break;

		case 2: 
			preempt = 160;
			break;

		case 3:
			preempt = 120;
			break;

		case 4: 
			preempt = 100;
			break;

		case 5:
			preempt = 80;
			break;

		case 6:
			preempt = 60;
			break;

		case 7:
			preempt = 40;
			break;

		case 8:
			preempt = 20;
			break;

		case 9:
			preempt = 10;
			break;

		default:
			preempt = 100;
			break;
	}


	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
	/* Old process returns here when resumed */

	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
