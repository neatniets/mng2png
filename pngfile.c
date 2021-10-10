#include <setjmp.h>

#include <png.h>

#include "pngfile.h"
#include "types.h"
#include "err.h"

// TODO: adding a write_row callback for progress meter.
// i don't necessarily want to add a curses dependency tho, so idk maybe not.

bool
pngf_init(
	struct pngfile *pfp,
	FILE *fp)
{
	pfp->fp = fp;
	// allocate the structures for libpng.
	// libpng recommends these be on the heap since they're large.
	pfp->write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
		NULL, NULL);
	if (pfp->write_ptr == NULL) {
		ERR("png_create_write_struct() failed.\n");
		goto error_write_ptr;
	}
	pfp->info_ptr = png_create_info_struct(pfp->write_ptr);
	if (pfp->info_ptr == NULL) {
		ERR("png_create_info_struct() failed.\n");
		goto error_info_ptr;
	}
	// initialize IO to the file specified.
	if (setjmp(png_jmpbuf(pfp->write_ptr))) {
		ERR("png_init_io() failed.\n");
		goto error_init_io;
	}
	png_init_io(pfp->write_ptr, fp);
	// pngset_write_status_fn() here if desired.
	// TODO: consider setting compression filters?

	return true;

error_init_io:
	png_destroy_info_struct(pfp->write_ptr, &pfp->info_ptr);
error_info_ptr:
	png_destroy_write_struct(&pfp->write_ptr, NULL);
error_write_ptr:
	return false;
}

void
pngf_clear(
	struct pngfile *pfp)
{
	png_destroy_write_struct(&pfp->write_ptr, &pfp->info_ptr);
}

bool
pngf_write_img(
	struct pngfile *pfp,
	struct image *imgp)
{
	// set up libpng's error system.
	if (setjmp(png_jmpbuf(pfp->write_ptr))) {
		ERR("couldn't write PNG file.\n");
		return false;
	}
	// set the metadata.
	png_set_IHDR(pfp->write_ptr, pfp->info_ptr,
		imgp->md.width, imgp->md.height,
		8, PNG_COLOR_TYPE_RGB_ALPHA,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);
	// TODO: if palette image types are introduced, set PLTE here.
	// would also want to utilize tRNS (alpha palette overlay).

	// set the rows in the info structure of libpng.
	png_set_rows(pfp->write_ptr, pfp->info_ptr,
		(png_bytepp)imgp->pixel_rows);
	// and send the image data to the file.
	png_write_png(pfp->write_ptr, pfp->info_ptr,
		PNG_TRANSFORM_IDENTITY, NULL);

	return true;
}
