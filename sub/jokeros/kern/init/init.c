#include <types.h>
#include <stdio.h>
#include <string.h>
#include <console.h>
#include <kdebug.h>
#include <picirq.h>
#include <trap.h>
#include <clock.h>
#include <intr.h>
#include <pmm.h>
#include <vmm.h>
#include <ide.h>
#include <swap.h>
#include <proc.h>
#include <sched.h>

int kern_init(void) __attribute__((noreturn));

int
kern_init(void) {
    extern char edata[], end[];
    memset(edata, 0, end - edata);
    cons_init();                // init the console

    cprintf("joker's os is loading.....\n\n");

    print_kerninfo();

    debug_init();               // init debug registers
    pmm_init();                 // init physical memory management

    pic_init();                 // init interrupt controller
    idt_init();                 // init interrupt descriptor table

	vmm_init();
	sched_init(); 				// init scheduler
	proc_init();				// init process table
	
	ide_init();
	swap_init();
	
    clock_init();               // init clock interrupt
    intr_enable();              // enable irq interrupt

    cpu_idle();					// run idle process
}

