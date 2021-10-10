#include <stdio.h>
#include <string.h>

#include "err.h"
#include "image.h"
#include "mngfile.h"
#include "types.h"
#include "verbose.h"
#include "pngfile.h"

// this buffer is statically allocated for the png filenames.
// the program will likely give up if a filename needs to be longer.
// TODO: allow a directory to be specified?
// platform-agnosticity is hard given varied path separators; could maybe force
// the user to end the dirname with the proper sep.
#define PNG_FNAME_BUF_LEN 1024
static char png_fname_buf[PNG_FNAME_BUF_LEN] = "";

// get the next png filename to use.
// simply numbers them starting from 0.
// if a filename can not fit, this will return NULL;
static char *
next_png_fname(void)
{
	static size_t count = 0;
	int nwritten = snprintf(png_fname_buf, PNG_FNAME_BUF_LEN,
		"%zu.png", count);
	if ((nwritten < 0) || (nwritten >= PNG_FNAME_BUF_LEN)) {
		ERRF("failed to get %zu-th png filename.\n", count);
		return NULL;
	}
	count++;
	return png_fname_buf;
}

static bool
init_mng(
	struct mngfile *mfp,
	const char *fname)
{
	// open mng file.
	FILE *mng_fp = fopen(fname, "rb");
	if (mng_fp == NULL) {
		ERRF("fopen(%s, rb) on mng file failed.\n", fname);
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

static bool
init_png(
	struct pngfile *pfp,
	const char *fname)
{
	FILE *png_fp = fopen(fname, "wb");
	if (png_fp == NULL) {
		ERRF("fopen(%s, wb) failed.\n", fname);
		return false;
	}
	if (!pngf_init(pfp, png_fp)) {
		ERR("couldn't init pngfile.\n");
		return false;
	}
	return true;
}

// TODO: this whole thing is spaghetti.
// i should really make it look less garbage.
static bool
write_frames(
	struct mngfile *mfp)
{
	// read metadata & create image struct.
	struct metadata md;
	mngf_read_md(mfp, &md);
	struct image img;
	img_init(&img, &md);

	// write each mng frame to a different png file.
	bool is_error = false;
	while (!is_error) {
		enum mngf_retcode rc = mngf_read_img(mfp, &img);
		if (rc != MNGF_OK) {
			is_error = (rc != MNGF_EOF);
			break;
		}

		// TODO: i think i can combine the above if-block with this one
		// with tricky boolean arithmetic.
		struct pngfile pngf;
		const char *png_fname = next_png_fname();
		if ((png_fname == NULL) || (!init_png(&pngf, png_fname))) {
			is_error = true;
			break;
		}

		if (!pngf_write_img(&pngf, &img)) {
			is_error = true;
		} else {
			// successful PNG write; leave a message.
			verbf("%s written.\n", png_fname);
		}
		// clear the pngfile for the next wave.
		// errors after the pngfile has been initialized will still
		// arrive here to clear it.
		pngf_clear(&pngf);
		fclose(pngf.fp);
	}

	img_clear(&img);
	return is_error;
}

int
main(
	int argc,
	char **argv)
{
	// open & initialize the mng file.
	const char mng_fname[] = "0000.mng"; // TODO: parse from argv.
	struct mngfile mngf;
	if (!init_mng(&mngf, mng_fname)) {
		return 1;
	}

	// write all the frames to png files.
	// rc is assigned 1 if the func fails and 0 if it succeeds.
	int rc = !write_frames(&mngf);

	mngf_clear(&mngf);
	fclose(mngf.fp);
	return rc;
}
