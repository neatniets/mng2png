#include "types.h"
#include "mng_callbacks.h"
#include "mng_userdata.h"

mng_ptr
mngcb_memalloc(
	mng_size_t sz)
{
	return calloc(1, sz);
}

void
mngcb_memfree(
	mng_ptr memptr,
	mng_size_t sz)
{
	free(memptr);
}

mng_bool
mngcb_openstream(
	mng_handle mh)
{
	// the userdata structure assumes the file has already been opened.
	// do nothing.
	return MNG_TRUE;
}

mng_bool
mngcb_closestream(
	mng_handle mh)
{
	mng_userdata *mudptr = mng_get_userdata(mh);
	mud_clear(mudptr); // safe if already been closed.
	return MNG_TRUE;
}

mng_bool
mngcb_readdata(
	mng_handle mh,
	mng_ptr buf,
	mng_uint32 buflen,
	mng_uint32p nwritten_ptr)
{
	mng_userdata *mudptr = mng_get_userdata(mh);
	*nwritten_ptr = fread(buf, 1, buflen, mudptr->fileptr);
	return MNG_TRUE;
}
