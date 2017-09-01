#include <stdio.h>
#include <ulib.h>

int
main(void) {
	fprintf(1, "Hello world!!.\n");
	fprintf(1, "I am process %d.\n", getpid());
	fprintf(1, "hello2 pass.\n");
	int i=10000;
	while(i--) { cprintf("."); }
	return 0;
}
