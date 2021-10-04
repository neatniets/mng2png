#ifndef MNG_CALLBACKS_H
#define MNG_CALLBACKS_H

#include <libmng.h>

mng_ptr
mngcb_memalloc(
	mng_size_t sz
);

void
mngcb_memfree(
	mng_ptr memptr,
	mng_size_t sz
);

#endif
