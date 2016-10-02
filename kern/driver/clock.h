#ifndef __KERN_DRIVER_CLOCK_H__
#define __KERN_DRIVER_CLOCK_H__

#include <types.h>
#include <list.h>

extern volatile size_t ticks;
extern volatile size_t tick_sec;

struct Timer {
	list_entry_t timer_link;
	int time;
	size_t tick;
};

void clock_init(void);
struct Timer* set_timer();
void free_timer(struct Timer *timer);
void traverse_Timer();

// convert list entry to Timer
#define le2Timer(le, member)                 \
	to_struct((le), struct Timer, member)

#endif /* !__KERN_DRIVER_CLOCK_H__ */
