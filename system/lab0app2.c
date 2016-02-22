#include <xinu.h>

process lab0app2(void) {
	int x = 5;
	x += 3;
	kprintf("arithmetic result: %d\n", x);
	int id = getpid();
	kprintf("%d\n", id);
}
