/**
 * main.c
 * Sample test cases for Lab 1, CS 354 Spring 2016.
 * @author  Xiangyu Bu <xb@purdue.edu>
 * @date    Feb 5, 2016
 */

#include <xinu.h>

volatile uint32  gcounter = 233333;

process counterproc(sid32 sem) {
  wait(sem); // Don't start until main tells so.
  while(gcounter > 0) {
    gcounter--;
  }
  return OK;
}

process test1 (void) {
	int i;
//	for (i = 0; i < 10; i++) {
	while(1){
		kprintf("test1\n");
	}
	return OK;
}

process myapp(void) {
  // TODO: do something to starve main process.
  while (1) {
	//do nothing forever
	sleep(1);
	kprintf("Starving the other processes\n");
  }
}

process main(void) {
  bool8 debug = 0;
  uint32 ts;
  bool8 flag = 0;
  sid32 sem = semcreate(0);
  pid32 pid = create(counterproc, 8192, INITPRIO, "test", 1, sem);

  if (proctab[pid].pr_cputime != 0) {
    kprintf("\033[91mpr_cputime init... FAILED\033[0m\r\n");
  } else {
    kprintf("\033[92mpr_cputime init... OK\033[0m\r\n");
  }

  resume(pid);
  if (proctab[pid].pr_tsready != clktime) {
    kprintf("\033[91mpr_tsready... FAILED\033[0m\r\n");
  } else {
    kprintf("\033[92mpr_tsready... OK\033[0m\r\n");
  }
  
  ts = clktime;
  signal(sem);
  while (ts == clktime) {
    if (proctab[pid].pr_tsready != clktime) {
      panic("\033[91mpr_tsready case 2... FAILED\033[0m\r\n");
    }
    if (!flag++) kputc('.');  // Don't print too many dots.
  }

  // When grading we will print the numbers and judge manually
  // rather than use conditions in this manner.
  // The numbers assume default boot overhead and default resched overhead.
  uint32 t = proctab[pid].pr_cputime + proctab[currpid].pr_cputime;
  if (t < 980 || t > 1020 || proctab[pid].pr_cputime > 200 ||
        proctab[pid].pr_cputime < 70) {
    kprintf("\033[91mpr_cputime update... FAILED\033[0m\r\n");
    kprintf("main ran %d ms.\r\n", proctab[currpid].pr_cputime);
    kprintf("test ran %d ms.\r\n", proctab[pid].pr_cputime);
  } else {
    kprintf("\033[92mpr_cputime update... OK\033[0m\r\n");
  }

  semdelete(sem);

  // Could test more things here.
  // Leave out on purpose.


  //Test starvation

  if (debug) {
	resume(create(myapp, 4096, INITPRIO, "myapp", 1, CONSOLE));
	kprintf("main: I am not starving!\r\n");
  }

  return OK;
}
