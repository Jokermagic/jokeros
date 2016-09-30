#include <stdio.h>
#include <string.h>
#include <trap.h>
#include <kmonitor.h>
#include <kdebug.h>
#include <mmu.h>
#include <memlayout.h>
#include <pmm.h>

/* *
 * Simple command-line kernel monitor useful for controlling the
 * kernel and exploring the system interactively.
 * */

struct command {
    const char *name;
    const char *desc;
    // return -1 to force monitor to exit
    int(*func)(int argc, char **argv, struct trapframe *tf);
};

static struct command commands[] = {
	{"hello", "hello world!", mon_hello},
    {"help", "Display this list of commands.", mon_help},
    {"kerninfo", "Display information about the kernel.", mon_kerninfo},
    {"backtrace", "Print backtrace of stack frame.", mon_backtrace},
	{"continue", "Continue execution", mon_continue},
	{"check_mem", "See the memory", mon_checkmem},
	{"malloc_test", "malloc_test", mon_malloc_test},
};

/* return if kernel is panic, in kern/debug/panic.c */
bool is_kernel_panic(void);

#define NCOMMANDS (sizeof(commands)/sizeof(struct command))

/***** Kernel monitor command interpreter *****/

#define MAXARGS         16
#define WHITESPACE      " \t\n\r"

/* parse - parse the command buffer into whitespace-separated arguments */
static int
parse(char *buf, char **argv) {
    int argc = 0;
    while (1) {
        // find global whitespace
        while (*buf != '\0' && strchr(WHITESPACE, *buf) != NULL) {
            *buf ++ = '\0';
        }
        if (*buf == '\0') break; 

        // save and scan past next arg
        if (argc == MAXARGS - 1) {
            cprintf("Too many arguments (max %d).\n", MAXARGS);
        }
        argv[argc ++] = buf;
        while (*buf != '\0' && strchr(WHITESPACE, *buf) == NULL) {
            buf ++;
        }
    }
    return argc;
}

/* *
 * runcmd - parse the input string, split it into separated arguments
 * and then lookup and invoke some related commands/
 * */
static int
runcmd(char *buf, struct trapframe *tf) {
    char *argv[MAXARGS];
    int argc = parse(buf, argv);
    if (argc == 0) return 0; 
    int i;
    for (i = 0; i < NCOMMANDS; i ++) {
        if (strcmp(commands[i].name, argv[0]) == 0) {
            return commands[i].func(argc - 1, argv + 1, tf);
        }
    }
    cprintf("Unknown command '%s'\n", argv[0]);
    return 0;
}

/***** Implementations of basic kernel monitor commands *****/

void
kmonitor(struct trapframe *tf) {
    cprintf("joker--Welcome to the kernel debug monitor!!\n");
    cprintf("joker--Type 'help' for a list of commands.\n");

    char *buf;
    while (1) {
        if ((buf = readline("@joker: ")) != NULL) {
            if (runcmd(buf, tf) < 0) break;
        }
    }
}

int mon_hello() {
	cprintf("hi, nice to meet you !\n");
}

/* mon_help - print the information about mon_* functions */
int
mon_help(int argc, char **argv, struct trapframe *tf) {
    int i;
    for (i = 0; i < NCOMMANDS; i ++) {
        cprintf("%s - %s\n", commands[i].name, commands[i].desc);
    }
    return 0;
}

/* *
 * mon_kerninfo - call print_kerninfo in kern/debug/kdebug.c to
 * print the memory occupancy in kernel.
 * */
int
mon_kerninfo(int argc, char **argv, struct trapframe *tf) {
    print_kerninfo();
    return 0;
}

/* *
 * mon_backtrace - call print_stackframe in kern/debug/kdebug.c to
 * print a backtrace of the stack.
 * */
int
mon_backtrace(int argc, char **argv, struct trapframe *tf) {
    print_stackframe();
    return 0;
}

/* mon_continue - continue execution if it isn't kernel panic */
int
mon_continue(int argc, char **argv, struct trapframe *tf) {
    if (is_kernel_panic()) {
        cprintf("can't continue execution in kernel panic.\n");
        return 0;
    }
    if (tf != NULL)  tf->tf_eflags &= ~FL_TF; 
    return -1;
}

int mon_checkmem() {
	print_pgdir();
}

int mon_malloc_test() {
		cprintf("mon_malloc_test\n");
//		struct Page *p = (struct Page *)alloc_pages(341);
//		struct Page *p1 = alloc_page();
//		struct Page *p2 = pgdir_alloc_page(boot_pgdir, 0, 1000);

		int * x1 = kmalloc( 30000 * sizeof(int));
		x1[0] = 2;
		x1[3] = 4;
		x1[4] = sizeof(x1);
		cprintf("%x", x1[4]);
		return 0;
}
