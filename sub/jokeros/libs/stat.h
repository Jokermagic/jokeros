#ifndef __LIBS_STAT_H__
#define __LIBS_STAT_H__

#include <types.h>

struct stat {
	uint32_t st_mode;
	size_t st_nlinks;
	size_t st_blocks;
	size_t st_size;
};


#endif /* !__LIBS_STAT_H__ */
