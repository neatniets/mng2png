#include <stdlib.h>
#include <stdint.h>

#include "err.h"
#include "image.h"
#include "malloc2.h"
#include "types.h"

// allocate memory for the pointers to rows of pixels.
// returns false if allocation failed; no freeing is necessary on failure.
static bool
alloc_row_ptrs(
	struct image *imgp)
{
	const size_t nrows = imgp->md.height;
	const size_t row_sz = sizeof(*imgp->pixel_rows);
	imgp->pixel_rows = malloc2(nrows, row_sz);
	if (imgp->pixel_rows == NULL) {
		ERRF("malloc2(%zu, %zu) failed.\n", nrows, row_sz);
		return false;
	}
	return true;
}
// free space from row pointers only.
// call AFTER freeing pixel rows.
static void
free_row_ptrs(
	struct image *imgp)
{
	free(imgp->pixel_rows);
}

// allocate memory for the rows.
// the row pointers must already be allocated.
// if false, allocation failed; no freeing is necessary.
static bool
alloc_rows(
	struct image *imgp)
{
	const size_t nrows = imgp->md.height;
	const size_t px_per_row = imgp->md.width;
	const size_t px_sz = sizeof(**imgp->pixel_rows);
	// allocate each row independently.
	// a single block for all pixels is theoretically an optimization, but
	// the system might have memory for separate rows and not a big block
	// and thus would fail on a big block where rows don't.
	// also, some systems might not fail on the malloc call even if there
	// isn't enough memory and will instead just kill the program e.g.
	// Linux.
	for (size_t i = 0; i < nrows; i++) {
		imgp->pixel_rows[i] = malloc2(px_per_row, px_sz);
		if (imgp->pixel_rows[i] == NULL) {
			// have to walk back through all the already-allocated
			// rows to free them...
			for (size_t j = 0; j < i; j++) {
				free(imgp->pixel_rows[j]);
			}
			ERRF("could not allocate %zu-th row of %zu pixels.\n",
				i, px_per_row);
			return false;
		}
	}
	return true;
}
// free memory occuppied by rows.
// call BEFORE freeing row pointers.
static void
free_rows(
	struct image *imgp)
{
	const size_t nrows = imgp->md.height;
	// each row is allocated individually.
	for (size_t i = 0; i < nrows; i++) {
		free(imgp->pixel_rows[i]);
	}
}

bool
img_init(
	struct image *imgp,
	const struct metadata *mdp)
{
	imgp->md = *mdp; // copy metadata.

	// allocate row pointers.
	if (!alloc_row_ptrs(imgp)) {
		goto error0;
	}
	// allocate rows of pixels.
	if (!alloc_rows(imgp)) {
		goto error1;
	}
	return true;

error1: // failed to allocate pixel rows.
	free_row_ptrs(imgp);
error0: // failed to allocate row pointers.
	ERR("could not alloc mem for image.\n");
	return false;
}

void
img_clear(
	struct image *imgp)
{
	free_rows(imgp);
	free_row_ptrs(imgp);
}
