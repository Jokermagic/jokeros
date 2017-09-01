#include <types.h>
#include <syscall.h>
#include <stdio.h>
#include <ulib.h>
#include <lock.h>

static lock_t fork_lock = INIT_LOCK;

void 
lock_fork(void) {
	lock(&fork_lock);
}

void 
unlock_fork(void) {
	unlock(&fork_lock);
}

void
exit(int error_code) {
    sys_exit(error_code);
    cprintf("BUG: exit failed.\n");
    while (1);
}

int
fork(void) {
	return sys_fork();
}

unsigned int
gettime_msec(void) {
	return (unsigned int)sys_gettime();
}

int
wait(void) {
	return sys_wait(0, NULL);
}

int 
waitpid(int pid, int *store) {
	return sys_wait(pid, store);
}

void 
yield(void) {
	sys_yield();
}

int 
sleep(unsigned int time) {
	return sys_sleep(time);
}

int 
kill(int pid) {
	return sys_kill(pid);
}

int
getpid(void) {
	return sys_getpid();
}

int
mmap(uintptr_t *addr_store, size_t len, uint32_t mmap_flags) {
	return sys_mmap(addr_store, len, mmap_flags);
}

int
munmap(uintptr_t addr, size_t len) {
    return sys_munmap(addr, len);
}

int
shmem(uintptr_t *addr_store, size_t len, uint32_t mmap_flags) {
    return sys_shmem(addr_store, len, mmap_flags);
}

sem_t
sem_init(int value) {
	return sys_sem_init(value);
}

int sem_post(sem_t sem_id) {
	return sys_sem_post(sem_id);
}

int sem_wait(sem_t sem_id) {
	return sys_sem_wait(sem_id, 0);
}

int sem_wait_timeout(sem_t sem_id, unsigned int timeout) {
	return sys_sem_wait(sem_id, timeout);
}

int 
sem_free(sem_t sem_id) {
	return sys_sem_free(sem_id);
}

int
sem_get_value(sem_t sem_id, int *value_store) {
	return sys_sem_get_value(sem_id, value_store);
}

int
send_event(int pid, int event) {
    return sys_send_event(pid, event, 0);
}

int
send_event_timeout(int pid, int event, unsigned int timeout) {
    return sys_send_event(pid, event, timeout);
}

int
recv_event(int *pid_store, int *event_store) {
    return sys_recv_event(pid_store, event_store, 0);
}

int
recv_event_timeout(int *pid_store, int *event_store, unsigned int timeout) {
    return sys_recv_event(pid_store, event_store, timeout);
}

int __clone(uint32_t clone_flags, uintptr_t stack, int (*fn)(void *), void *arg);

int
clone(uint32_t clone_flags, uintptr_t stack, int (*fn)(void *), void *arg) {
    int ret;
    lock_fork();
    ret = __clone(clone_flags, stack, fn, arg);
    unlock_fork();
    return ret;
}


