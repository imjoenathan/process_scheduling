#include <xinu.h>

process lab01app1(void) {
	printf("I want an A in this class!\n");
	int id = getpid();
	if (id >= 0) {
		printf("%d\n", id);
	}
}
