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

// args from the commandline after parsing.
static struct {
	char *prog_name;
	char *mng_fname;
} args;

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
		return false;
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

static void
print_help(void)
{
	const char fmt[]
		= "usage:\n"
		"%s [-v] (-h | <mng-filename>)\n"
		"\t-v - be verbose.\n"
		"\t-h - print this help text.\n";
	printf(fmt, args.prog_name); // prog_name should be set.
}

// parses the commandline arguments.
static bool
parse_args(
	int argc,
	char **argv)
{
	// TODO: should command-line errors give source line numbers like other
	// errors?
	// it's meant for program bugs to be detected, not necessarily user
	// error.
	// if the user enters the command wrong, it's not an error in the source
	// code and clutters stdout.
	// however, user error could also result in errors down the road, like a
	// failed libmng initialization, so maybe there's no real way to keep
	// the source code out of stdout on user error.
	args.prog_name = *argv;
	argv++;
	argc--;

	// options parsing.
	while ((*argv != NULL) && (**argv == '-')) {
		char *cur = *argv;
		switch (cur[1]) {
		case 'h':
			print_help();
			// TODO: getting help should exit immediately but not in
			// error.
			return false;
		case 'v':
			is_verbose = true;
			break;
		default:
			ERRF("bad option: -%c.\n", cur[1]);
			return false;
		}
		argv++;
		argc--;
	}

	// required args parsing.
	// TODO: allow more than one MNG file to be specified at a time.
	if (*argv == NULL) {
		ERR("specify an MNG file to convert.\n");
		return false;
	}
	args.mng_fname = *argv;
	argv++;
	argc--;

	if (argc > 0) {
		verbf("last %d arg(s) ignored.\n", argc);
	}
	return true;
}

int
main(
	int argc,
	char **argv)
{
	// parse commandline args.
	if (!parse_args(argc, argv)) {
		return 1;
	}
	// open & initialize the mng file.
	struct mngfile mngf;
	if (!init_mng(&mngf, args.mng_fname)) {
		return 1;
	}

	// write all the frames to png files.
	// rc is assigned 1 if the func fails and 0 if it succeeds.
	int rc = !write_frames(&mngf);

	mngf_clear(&mngf);
	fclose(mngf.fp);
	return rc;
}
