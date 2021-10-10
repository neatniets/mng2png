#include <stdio.h>

#include "err.h"
#include "image.h"
#include "mngfile.h"
#include "types.h"
#include "verbose.h"
#include "pngfile.h"

static bool
init_mng(
	struct mngfile *mfp,
	int *argcp,
	char ***argvp)
{
	const char mng_fname[] = "0000.mng"; // TODO: parse from argv.
	// open mng file.
	FILE *mng_fp = fopen(mng_fname, "rb");
	if (mng_fp == NULL) {
		ERRF("fopen(%s, rb) on mng file failed.\n", mng_fname);
		return 1;
	}
	// initialize mngfile structure from open file.
	enum mngf_retcode rc = mngf_init(mfp, mng_fp);
	if (rc != MNGF_OK) {
		ERR("mngf_init() failed.\n");
		return false; // exiting, so no frees required.
	}

	return true;
}

int
main(
	int argc,
	char **argv)
{
	struct mngfile mngf;
	if (!init_mng(&mngf, &argc, &argv)) {
		return 1;
	}
	// read metadata & create image struct.
	struct metadata md;
	mngf_read_md(&mngf, &md);
	struct image img;
	img_init(&img, &md);

	// test only a single png.
	const char png_fname[] = "tmp.png";
	FILE *png_fp = fopen(png_fname, "wb");
	if (png_fp == NULL) {
		ERRF("fopen(%s, wb) failed.\n", png_fname);
		return 1;
	}
	struct pngfile pngf;
	if (!pngf_init(&pngf, png_fp)) {
		ERR("couldn't init pngfile.\n");
		return 1;
	}

	if (mngf_read_img(&mngf, &img) != MNGF_OK) {
		ERR("failed to read mng frame.\n");
		return 1;
	}
	if (!pngf_write_img(&pngf, &img)) {
		ERR("failed to write png.\n");
		return 1;
	}

	img_clear(&img);
	pngf_clear(&pngf);
	fclose(pngf.fp);
	mngf_clear(&mngf);
	fclose(mngf.fp);
	return 0;
}
