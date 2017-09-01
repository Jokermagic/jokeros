#include <stdio.h>
#include <ulib.h>

int user_hello()
{
	cprintf("user_hello\n");
}

int 
main(void) {
	cprintf("Hello world!!.\n");
	sys_hello(user_hello);
//	user_hello();
//	int(*func)() = user_hello;
//	func();
//	cprintf("     userarg:%x\n", func);
	return 0;
}
