#include <fs.h>
#include <assert.h>
#include <file.h>
#include <slab.h>
#include <stdio.h>

struct fs_struct *
fs_create(void) {
	static_assert((int)FS_STRUCT_NENTRY > 128);
	struct fs_struct *fs_struct;
	if ((fs_struct = kmalloc(sizeof(struct fs_struct) + FS_STRUCT_BUFSIZE)) != NULL) {
		fs_struct->pwd = NULL;
		fs_struct->filemap = (void *)(fs_struct + 1);
		atomic_set(&(fs_struct->fs_count), 0);
		sem_init(&(fs_struct->fs_sem), 1);
		filemap_init(fs_struct->filemap);
	}
	return fs_struct;
}
	
void fs_destroy(struct fs_struct *fs_struct);
int dup_fs(struct fs_struct *to, struct fs_struct *from);
