#ifndef MNGFILE_H
#define MNGFILE_H

#include "types.h"

// initialize the mngfile for use.
// file pointer must be opened for reading a binary file.
enum mngf_retcode
mngf_init(
	struct mngfile *mfp,
	FILE *fp);

// called to free up the mngfile when it's no longer needed.
void
mngf_clear(
	struct mngfile *mfp);

// read metadata from the MNG file.
// needs to only be called once before reading through all images.
enum mngf_retcode
mngf_read_md(
	struct mngfile *mfp,
	struct metadata *mdp);

// reads pixel data of the next frame into the image structure.
// will return MNGF_EOF when all frames have been read from the file.
enum mngf_retcode
mngf_read_img(
	struct mngfile *mfp,
	struct image *imgp);

#endif
