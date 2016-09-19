#ifndef __KERN_FS_DEVS_DEV_H__
#define __KERN_FS_DEVS_DEV_H__

#include <types.h>
#include <iobuf.h>

struct device {
	size_t d_blocks;
	size_t d_blocksize;
	int (*d_open)(struct device *dev, uint32_t open_flags);
	int (*d_close)(struct device *dev);
	int (*d_io)(struct device *dev, struct iobuf *iob, bool write);
	int (*d_ioctl)(struct device *dev, int op, void *data);
};

#endif /* !__KERN_FS_DEVS_DEV_H__ */
