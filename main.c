#include <stdio.h>

#include "err.h"
#include "image.h"
#include "mngfile.h"
#include "types.h"
#include "verbose.h"

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

	// read metadata & create image struct.
	struct metadata md;
	mngf_read_md(&mngf, &md);
	struct image img;
	img_init(&img, &md);

	size_t num_frames = 0;
	do {
		rc = mngf_read_img(&mngf, &img);
		num_frames++;
	} while (rc == MNGF_OK);
	if (rc == MNGF_ERR) {
		ERRF("failed to read %zu-th frame.\n", num_frames);
		return 1;
	}
	verbf("%zu MNG frames read.\n", num_frames);

	mngf_clear(&mngf);
	img_clear(&img);
	fclose(mng_fp);
	return 0;
}
