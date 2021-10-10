#include <stdio.h>

#include "err.h"
#include "image.h"
#include "mngfile.h"
#include "types.h"

int
main(
	int argc,
	char *argv[])
{
	const char mng_fname[] = "0000.mng"; // TODO: parse from argv.
	// open mng file.
	FILE *mng_fp = fopen(mng_fname, "r");
	if (mng_fp == NULL) {
		ERRF("fopen(%s, r) on mng file failed.\n", mng_fname);
		return 1;
	}
	// initialize mngfile structure from open file.
	struct mngfile mngf;
	enum mngf_retcode rc = mngf_init(&mngf, mng_fp);
	if (rc != MNGF_OK) {
		ERR("mngf_init() failed.\n");
		return 1; // exiting, so no frees required.
	}

	// for now, just test destruction of mngfile structure.
	mngf_clear(&mngf);

	fclose(mng_fp);
	return 0;
}
