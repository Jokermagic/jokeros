#include <sysfile.h>
#include <stdio.h>
#include <proc.h>
#include <file.h>
#include <error.h>
#include <slab.h>
#include <vmm.h>

#define IOBUF_SIZE					4096

int sysfile_open(const char *path, uint32_t open_flags) {
	return 0;
}
int sysfile_close(int fd) {
	return 0;
}
int sysfile_read(int fd, void *base, size_t len) {
	return 0;
}
int sysfile_write(int fd, void *base, size_t len) {
	struct mm_struct *mm = current->mm;
	if (len == 0) { return 0; }
	if (!file_testfd(fd, 0, 1)) { return -E_INVAL; }
	void *buffer;
	if ((buffer = kmalloc(IOBUF_SIZE)) == NULL) { return -E_NO_MEM; }
	int ret = 0;
	size_t copied = 0, alen;
	while (len != 0) {
		if ((alen = IOBUF_SIZE) > len) { alen = len; }
		lock_mm(mm);
		{	/*copy_from_user is used to copy memory to buffer*/
			if (!copy_from_user(mm, buffer, base, alen, 0))
				ret = -E_INVAL;
		}
		unlock_mm(mm);
		if (ret == 0) {	/*file_write is used to write*/
			ret = file_write(fd, buffer, alen, &alen);
			if (alen != 0) {
				assert(len >= alen);
				base += alen, len -= alen, copied +=alen;
			}
		}
		if (ret != 0 || alen == 0)
			goto out;
	}
out:
	kfree(buffer);
	if (copied != 0) { return copied; }
	return ret;
}
int sysfile_fstat(int fd, struct stat*stat) {
	return 0;
}
int sysfile_dup(int fd1, int fd2) {
	return 0;
}
