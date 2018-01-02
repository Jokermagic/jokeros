#include <inode.h>
#include <assert.h>
#include <error.h>
#include <stdio.h>

void
inode_init(struct inode *node, const struct inode_ops *ops, struct fs *fs) {
	atomic_set(&(node->ref_count), 0);
	atomic_set(&(node->open_count), 0);
	node->in_ops = ops, node->in_fs = fs;
	vop_ref_inc(node);
}

/* *
 * inode_ref_inc - increment ref_count
 * invoked by vop_ref_inc
 * */
int
inode_ref_inc(struct inode *node) {
    return atomic_add_return(&(node->ref_count), 1);
}

/* *
 * inode_ref_dec - decrement ref_count
 * invoked by vop_ref_dec
 * calls vop_reclaim if the ref_count hits zero
 * */
int
inode_ref_dec(struct inode *node) {
    assert(inode_ref_count(node) > 0);
    int ref_count, ret;
    if ((ref_count = atomic_sub_return(&(node->ref_count), 1)) == 0) {
        if ((ret = vop_reclaim(node)) != 0 && ret != -E_BUSY) {
            cprintf("vfs: warning: vop_reclaim: %e.\n", ret);
        }
    }
    return ref_count;
}

/* *
 * inode_open_inc - increment the open_count
 * invoked by vop_open_inc
 * */
int
inode_open_inc(struct inode *node) {
    return atomic_add_return(&(node->open_count), 1);
}

/* *
 * inode_open_dec - decrement the open_count
 * invoked by vop_open_dec
 * calls vop_close if the open_count hits zero
 * */
int
inode_open_dec(struct inode *node) {
    assert(inode_open_count(node) > 0);
    int open_count, ret;
    if ((open_count = atomic_sub_return(&(node->open_count), 1)) == 0) {
        if ((ret = vop_close(node)) != 0) {
            cprintf("vfs: warning: vop_close: %e.\n", ret);
        }
    }
    return open_count;
}


void 
inode_check(struct inode *node, const char *opstr) {
	assert(node != NULL && node->in_ops != NULL);
	assert(node->in_ops->vop_magic == VOP_MAGIC);
	int ref_count = inode_ref_count(node), open_count = inode_open_count(node);
	assert(ref_count >= open_count && open_count >= 0);
	assert(ref_count < MAX_INODE_COUNT && open_count < MAX_INODE_COUNT);
}