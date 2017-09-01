#ifndef __KERN_SYNC_MONITOR_CONDVAR_H__
#define __KERN_SYNC_MONITOR_CONDVAR_H__

#include <sem.h>

typedef struct monitor monitor_t;

typedef struct condvar {
	semaphore_t sem;        // the sem semaphore  is used to down the waiting proc, and the signaling proc should up the waiting proc
	int count;				// the number of waiters on condvar
	monitor_t * owner;		// the owner(monitor) of this condvar
} condvar_t;

typedef struct monitor {
	semaphore_t mutex;		// the mutex lock for going into the routines in monitor, sould be initialized to 1
	semaphore_t next;		// the next semaphore is used to down the signaling proc itself, and the other OR wakeuped waiting proc should wake up the sleeped signaling proc.
	int next_count;			// the number of sleeped signaling proc
	condvar_t *cv;			// the condvars in monitor
} monitor_t;

void monitor_init (monitor_t *cvp, size_t num_cv);
void cond_signal (condvar_t *cvp);
void cond_wait (condvar_t *cvp);

#endif /* ! __KERN_SYNC_MONITOR_CONDVAR_H__ */
