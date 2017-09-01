#include <file.h>
#include <proc.h>
#include <assert.h>
#include <fs.h>
#include <error.h>
#include <iobuf.h>

#define testfd(fd)				((fd) >= 0 && (fd) < FS_STRUCT_NENTRY)

static struct file *
get_filemap(void) {
	struct fs_struct *fs_struct = current->fs_struct;
	assert(fs_struct != NULL && fs_count(fs_struct) > 0);
	return fs_struct->filemap;
}

void filemap_init(struct file *filemap) {
	int fd;
	struct file *file = filemap;
	for (fd = 0; fd < FS_STRUCT_NENTRY; fd ++, file ++) {
		atomic_set(&(file->open_count), 0);
		file->status = FD_NONE, file->fd = fd;
	}
}

static void
filemap_acquire(struct file *file) {
	assert(file->status == FD_OPENED);
	fopen_count_inc(file);
}

void filemap_open(struct file *file);
void filemap_close(struct file *file);
void filemap_dup(struct file *to, struct file *from) {
}

static inline int
fd2file(int fd, struct file **file_store) {
	/* make sure fd > 0 and fd < pgsize - sizeof(fs_struct) */
	if (testfd(fd)) {
		struct file *file = get_filemap() + fd;
		if (file->status == FD_OPENED && file->fd == fd) {
			*file_store = file;
			return 0;
		}
	}
	return -E_INVAL;
}

bool 
file_testfd(int fd, bool readable, bool writable) {
	int ret;
	struct file *file;
	if ((ret = fd2file(fd, &file)) != 0) { return 0; }
	if (readable && !file->readable) { return 0; } 
	if (writable && !file->writable) { return 0; }
	return 1;
}

int file_open(char *path, uint32_t open_flags);
int file_close(int fd);
int file_read(int fd, void *base, size_t len, size_t *copied_store);
int file_write(int fd, void *base, size_t len, size_t *copied_store) {
	int ret;
	struct file *file;
	*copied_store = 0;
	if ((ret = fd2file(fd, &file)) != 0) { return ret; }
	if (!file->writable) { return -E_INVAL; }
	filemap_acquire(file);
	
	struct iobuf __iob, *iob = iobuf_init(&__iob, base, len, file->pos);
}
int file_fstat(int fd, struct stat *stat);
int file_dup(int fd1, int fd2);
