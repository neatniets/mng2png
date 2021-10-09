#ifndef IMAGE_H
#define IMAGE_H

#include "types.h"

// initialize an image against the given metadata.
bool
img_init(
	struct image *imgp,
	const struct metadata *mdp);

// free up memory for an image no longer in use.
void
img_clear(
	struct image *imgp);

#endif
