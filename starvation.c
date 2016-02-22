/**
 * main.c
 */
#include <xinu.h>

process myapp(void) {
  // TODO: do something to starve main process.
  while (1) {
	//do nothing forever
  }
}

process main(void) {
  // TODO: create process that run myapp
  // The following code must never run.

  resume(create(myapp, 4096, 9999, "myapp", 1, CONSOLE));
  kprintf("main: I am not starving!\r\n");
  return OK;
}
