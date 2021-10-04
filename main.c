#include <stddef.h>
#include <libmng.h>
#include <stdio.h>

#include "mng_callbacks.h"

int
main(
	int argc,
	char *argv[]
) {
	// initialize handle for libmng.
	mng_handle mh = mng_initialize(MNG_NULL, mngcb_memalloc, mngcb_memfree,
		MNG_NULL);
	if (mh == MNG_NULL) {
		fputs("main: could not obtain libmng handle.\n", stderr);
	}

	// clean up libmng handle.
	mng_cleanup(&mh);

	return 0;
}
