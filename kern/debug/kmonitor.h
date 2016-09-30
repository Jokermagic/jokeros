#ifndef __KERN_DEBUG_MONITOR_H__
#define __KERN_DEBUG_MONITOR_H__

#include <trap.h>

void kmonitor(struct trapframe *tf);

int mon_hello();
int mon_help(int argc, char **argv, struct trapframe *tf);
int mon_kerninfo(int argc, char **argv, struct trapframe *tf);
int mon_backtrace(int argc, char **argv, struct trapframe *tf);
int mon_continue(int argc, char **argv, struct trapframe *tf);
int mon_checkmem();
int mon_malloc_test();

#endif /* !__KERN_DEBUG_MONITOR_H__ */

