#ifndef __KERN_FS_FS_H__
#define __KERN_FS_FS_H__

#include <mmu.h>
#include <atomic.h>
#include <sem.h>

#define SECTSIZE            512
#define PAGE_NSECT          (PGSIZE / SECTSIZE)

#define SWAP_DEV_NO         1

struct fs_struct {
	struct inode *pwd;
	struct file *filemap;
	atomic_t fs_count;
	semaphore_t fs_sem;
};

#define FS_STRUCT_BUFSIZE				(PGSIZE - sizeof(struct fs_struct))
#define FS_STRUCT_NENTRY				(FS_STRUCT_BUFSIZE / sizeof(struct file))

struct fs_struct *fs_create(void);
void fs_destroy(struct fs_struct *fs_struct);
int dup_fs(struct fs_struct *to, struct fs_struct *from);

static inline int
fs_count(struct fs_struct *fs_struct) {
	return atomic_read(&(fs_struct->fs_count));
}
static inline int
fs_count_inc(struct fs_struct *fs_struct) {
	return atomic_add_return(&(fs_struct->fs_count), 1);
}
static inline int
fs_count_dec(struct fs_struct *fs_struct) {
	return atomic_sub_return(&(fs_struct->fs_count), 1);
}

#endif /* !__KERN_FS_FS_H__ */

