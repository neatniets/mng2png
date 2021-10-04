#include "mng_callbacks.h"

mng_ptr
mngcb_memalloc(
	mng_size_t sz
) {
	return calloc(1, sz);
}

void
mngcb_memfree(
	mng_ptr memptr,
	mng_size_t sz
) {
	free(memptr);
}
