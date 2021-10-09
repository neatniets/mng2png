#include <stdlib.h>
#include <stdint.h>

#include "malloc2.h"
#include "err.h"

void *
malloc2(
	size_t nmemb,
	size_t memb_sz)
{
	// if (a*b > max) is true, then so is (a > max/b).
	// this detects overflow and returns nothing in that event.
	if (nmemb > SIZE_MAX / memb_sz) {
		ERRF("%zu * %zu would overflow.\n", nmemb, memb_sz);
		return NULL;
	}
	// then just do a malloc cuz we know it won't overflow.
	return malloc(nmemb * memb_sz);
}
