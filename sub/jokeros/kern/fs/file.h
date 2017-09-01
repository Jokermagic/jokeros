#ifndef __KERN_FS_FILE_H__
#define __KERN_FS_FILE_H__

#include <types.h>
#include <atomic.h>

struct stat;

struct file {
	enum {
		FD_NONE, FD_INIT, FD_OPENED, FD_CLOSED,
	} status;
	bool readable;
	bool writable;
	int fd;
	off_t pos;
	struct inode *node;
	atomic_t open_count;
};

void filemap_init(struct file *filemap);
void filemap_open(struct file *file);
void filemap_close(struct file *file);
void filemap_dup(struct file *to, struct file *from);
bool file_testfd(int fd, bool readable, bool writable);

int file_open(char *path, uint32_t open_flags);
int file_close(int fd);
int file_read(int fd, void *base, size_t len, size_t *copied_store);
int file_write(int fd, void *base, size_t len, size_t *copied_store);
int file_fstat(int fd, struct stat *stat);
int file_dup(int fd1, int fd2);

static inline int
fopen_count(struct file *file) {
	return atomic_read(&(file->open_count));
}
static inline int
fopen_count_inc(struct file *file) {
	return atomic_add_return(&(file->open_count), 1);
}
static inline int
fopen_count_dec(struct file *file) {
	return atomic_sub_return(&(file->open_count), 1);
}

#endif /* !__KERN_FS_FILE_H__ */
