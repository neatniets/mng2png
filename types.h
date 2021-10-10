#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include <libmng.h>
#include <png.h>

#if (CHAR_BIT != 8)
#	error "8-bit bytes are required."
#endif
typedef unsigned char byte;

// represents a single pixel.
// it is an array with predefined indices instead of a structure with named
// fields due to the fact that the actual order (in memory) of structure fields
// is not defined.
typedef byte pixel[4];
enum px_index {
	PX_RED = 0,
	PX_GREEN = 1,
	PX_BLUE = 2,
	PX_ALPHA = 3,
};
// information defining the structure of the file e.g. its resolution.
// normally obtained before reading pixel data.
struct metadata {
	size_t height;
	size_t width;
};
// structure holding all data necessary to reconstruct an image.
struct image {
	struct metadata md;
	pixel **pixel_rows;
};

// data needed to read an MNG file.
struct mngfile {
	FILE *fp;
	mng_handle mh;
	struct image *imgp; // ptr to image to be written to.
	// ticks starts at 0 and only increments to a value that libmng asks for
	// to draw the next frame since we're "reading" frames instead of
	// actually displaying them.
	mng_uint32 ticks;
	// metadata might want to be obtained a second time, but the
	// processheader() callback is only called once and thus can only fill a
	// single metadata object.
	// this is circumvented by caching the metadata and just assigning this
	// value to any requested memory location.
	struct metadata cached_md;
	// libmng has a separate function mng_display_resume() to be called
	// after you've started displaying with mng_display() for some reason.
	// this flag tells the file which function to call.
	bool has_started_displaying : 1;
	bool is_eof : 1;
};
enum mngf_retcode {
	MNGF_OK,
	MNGF_ERR,
	MNGF_EOF,
};

// data needed to write a PNG file.
struct pngfile {
	FILE *fp;
	png_structp write_ptr;
	png_infop info_ptr;
};

#endif
