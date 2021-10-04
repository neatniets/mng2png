#ifndef MNG_CALLBACKS_H
#define MNG_CALLBACKS_H

#include <libmng.h>

#define MNG_SETCB(MH, CB_NAME) \
	mng_setcb_##CB_NAME(MH, mngcb_##CB_NAME)

mng_ptr
mngcb_memalloc(
	mng_size_t sz);

void
mngcb_memfree(
	mng_ptr memptr,
	mng_size_t sz);

mng_bool
mngcb_openstream(
	mng_handle mh);

mng_bool
mngcb_closestream(
	mng_handle mh);

mng_bool
mngcb_readdata(
	mng_handle mh,
	mng_ptr buf,
	mng_uint32 buflen,
	mng_uint32p nwritten_ptr);

#endif
