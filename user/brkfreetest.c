#include <ulib.h>
#include <stdio.h>

const int max_child = 32;

int 
main(void) {
	int n, pid;
	for (n = 0; n < max_child; n ++) {
		if ((pid = fork()) == 0) {
			cprintf("I am child %d\n", n);
		}
		assert(pid > 0);
	}
	return 0;
}
