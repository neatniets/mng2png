#include "types.h"
#include "err.h"
#include "image.h"

int
main(
	int argc,
	char *argv[])
{
	// testing image funcs.
	struct metadata md = {
		.height = 1600,
		.width = 900,
	};
	struct image img;
	if (!img_init(&img, &md)) {
		ERRF("could not load %zux%zu image.\n", md.height, md.width);
		return 1;
	}
	img_clear(&img);
	// testing error func.
	ERR("image successfully loaded.\n");
	return 0;
}
