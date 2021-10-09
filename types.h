#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include <libmng.h>

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

#endif
