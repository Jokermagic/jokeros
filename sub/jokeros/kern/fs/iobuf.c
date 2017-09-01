#include <iobuf.h>

struct iobuf * 
iobuf_init(struct iobuf *iob, void *base, size_t len, off_t offset) {
	iob->io_base = base;
	iob->io_offset = offset;
	iob->io_len = iob->io_resid = len;
	return iob;
}

int 
iobuf_move(struct iobuf *iob, void *data, size_t len, bool m2b, size_t *copiedp) {
	return 0;
}
