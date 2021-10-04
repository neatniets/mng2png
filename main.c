#include <stddef.h>
#include <libmng.h>
#include <stdio.h>

#include "types.h"
#include "mng_callbacks.h"
#include "mng_userdata.h"

int
main(
	int argc,
	char *argv[])
{
	// prepare userdata structure.
	const string mng_fname = "0000.mng"; // TODO: replace with cmdline.
	mng_userdata ud;
	mud_init(&ud);
	if (!mud_prepare(&ud, mng_fname)) {
		fputs("main: failed to prepare userdata.\n", stderr);
		return EXIT_FAILURE;
	}

	// initialize handle for libmng.
	mng_handle mh = mng_initialize(&ud, mngcb_memalloc, mngcb_memfree,
		MNG_NULL);
	if (mh == MNG_NULL) {
		fputs("main: could not obtain libmng handle.\n", stderr);
		return EXIT_FAILURE;
	}
	// set callbacks required for reading.
	MNG_SETCB(mh, openstream);
	MNG_SETCB(mh, closestream);
	MNG_SETCB(mh, readdata);

	// clean up.
	mud_clear(&ud);
	mng_cleanup(&mh);

	return EXIT_SUCCESS;
}
