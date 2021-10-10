#ifndef PNGFILE_H
#define PNGFILE_H

#include "types.h"

// initialize a PNG file to use the designed file pointer.
// the file must be opened for writing to binary.
// if this returns false, the PNG file was not initialized; no freeing required.
bool
pngf_init(
	struct pngfile *pfp,
	FILE *fp);

// clear out a pngfile no longer in use.
void
pngf_clear(
	struct pngfile *pfp);

// write an image structure to the PNG file.
// if false, a valid file is not written.
bool
pngf_write_img(
	struct pngfile *pfp,
	struct image *imgp);

#endif
