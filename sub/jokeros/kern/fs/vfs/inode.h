#ifndef __KERN_FS_VFS_INODE_H__
#define __KERN_FS_VFS_INODE_H__

#include <types.h>
#include <dev.h>
#include <atomic.h>

struct stat;
struct iobuf;

/*
 * Abstract low-level file.
 *
 * Note: in_info is Filesystem-specific data, in_type is the inode type
 *
 * open_count is managed using VOP_INCOPEN and VOP_DECOPEN by
 * vfs_open() and vfs_close(). Code above the VFS layer should not
 * need to worry about it.
 */
struct inode {
	union {
		struct device __device_info;
	} in_info;
	enum {
		inode_type_device_info = 0x1234,
	} in_type;
	atomic_t ref_count;
	atomic_t open_count;
	struct fs *in_fs;
	const struct inode_ops *in_ops;
};

int inode_ref_inc(struct inode *node);
int inode_ref_dec(struct inode *node);
int inode_open_inc(struct inode *node);
int inode_open_dec(struct inode *node);

#define MAX_INODE_COUNT			0x10000
#define VOP_MAGIC 				0x8c4ba476

struct inode_ops {
    unsigned long vop_magic;
    int (*vop_open)(struct inode *node, uint32_t open_flags);
    int (*vop_close)(struct inode *node);
    int (*vop_read)(struct inode *node, struct iobuf *iob);
    int (*vop_write)(struct inode *node, struct iobuf *iob);
    int (*vop_fstat)(struct inode *node, struct stat *stat);
    int (*vop_fsync)(struct inode *node);
    int (*vop_mkdir)(struct inode *node, const char *name);
    int (*vop_link)(struct inode *node, const char *name, struct inode *link_node);
    int (*vop_rename)(struct inode *node, const char *name, struct inode *new_node, const char *new_name);
    int (*vop_readlink)(struct inode *node, struct iobuf *iob);
    int (*vop_symlink)(struct inode *node, const char *name, const char *path);
    int (*vop_namefile)(struct inode *node, struct iobuf *iob);
    int (*vop_getdirentry)(struct inode *node, struct iobuf *iob);
    int (*vop_reclaim)(struct inode *node);
    int (*vop_ioctl)(struct inode *node, int op, void *data);
    int (*vop_gettype)(struct inode *node, uint32_t *type_store);
    int (*vop_tryseek)(struct inode *node, off_t pos);
    int (*vop_truncate)(struct inode *node, off_t len);
    int (*vop_create)(struct inode *node, const char *name, bool excl, struct inode **node_store);
    int (*vop_unlink)(struct inode *node, const char *name);
    int (*vop_lookup)(struct inode *node, char *path, struct inode **node_store);
    int (*vop_lookup_parent)(struct inode *node, char *path, struct inode **node_store, char **endp);
};

/* Consistency check */
void inode_check(struct inode *node, const char *opstr);

#define __vop_op(node, sym)                                                                         \
	({                                                                                              \
        struct inode *__node = (node);                                                              \
        assert(__node != NULL && __node->in_ops != NULL && __node->in_ops->vop_##sym != NULL);      \
        inode_check(__node, #sym);                                                                  \
        __node->in_ops->vop_##sym;                                                                  \
  })

#define vop_open(node, open_flags)                                  (__vop_op(node, open)(node, open_flags))
#define vop_close(node)                                             (__vop_op(node, close)(node))
#define vop_read(node, iob)                                         (__vop_op(node, read)(node, iob))
#define vop_write(node, iob)                                        (__vop_op(node, write)(node, iob))
#define vop_fstat(node, stat)                                       (__vop_op(node, fstat)(node, stat))
#define vop_fsync(node)                                             (__vop_op(node, fsync)(node))
#define vop_mkdir(node, name)                                       (__vop_op(node, mkdir)(node, name))
#define vop_link(node, name, link_node)                             (__vop_op(node, link)(node, name, link_node))
#define vop_rename(node, name, new_node, new_name)                  (__vop_op(node, rename)(node, name, new_node, new_name))
#define vop_readlink(node, iob)                                     (__vop_op(node, readlink)(node, iob))
#define vop_symlink(node, name, path)                               (__vop_op(node, symlink)(node, name, path))
#define vop_namefile(node, iob)                                     (__vop_op(node, namefile)(node, iob))
#define vop_getdirentry(node, iob)                                  (__vop_op(node, getdirentry)(node, iob))
#define vop_reclaim(node)                                           (__vop_op(node, reclaim)(node))
#define vop_ioctl(node, op, data)                                   (__vop_op(node, ioctl)(node, op, data))
#define vop_gettype(node, type_store)                               (__vop_op(node, gettype)(node, type_store))
#define vop_tryseek(node, pos)                                      (__vop_op(node, tryseek)(node, pos))
#define vop_truncate(node, len)                                     (__vop_op(node, truncate)(node, len))
#define vop_create(node, name, excl, node_store)                    (__vop_op(node, create)(node, name, excl, node_store))
#define vop_unlink(node, name)                                      (__vop_op(node, unlink)(node, name))
#define vop_lookup(node, path, node_store)                          (__vop_op(node, lookup)(node, path, node_store))
#define vop_lookup_parent(node, path, node_store, endp)             (__vop_op(node, lookup_parent)(node, path, node_store, endp))

#define vop_fs(node)                                                ((node)->in_fs)
#define vop_init(node, ops, fs)                                     inode_init(node, ops, fs)
#define vop_kill(node)                                              inode_kill(node)

/*
 * Reference count manipulation (handled above filesystem level)
 */
#define vop_ref_inc(node)                                           inode_ref_inc(node)
#define vop_ref_dec(node)                                           inode_ref_dec(node)
/*
 * Open count manipulation (handled above filesystem level)
 *
 * VOP_INCOPEN is called by vfs_open. VOP_DECOPEN is called by vfs_close.
 * Neither of these should need to be called from above the vfs layer.
 */
#define vop_open_inc(node)                                          inode_open_inc(node)
#define vop_open_dec(node)                                          inode_open_dec(node)

static inline int
inode_ref_count(struct inode *node) {
	return atomic_read(&(node->ref_count));
}
static inline int
inode_open_count(struct inode *node) {
	return atomic_read(&(node->open_count));
}

#endif /* !__KERN_FS_VFS_INODE_H__ */
