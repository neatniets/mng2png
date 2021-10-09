#ifndef MALLOC2_H
#define MALLOC2_H

#include <stddef.h>

// malloc but with parameters like calloc and fails on overflow.
// using malloc() just multiplying the two values might overflow and
// successfully allocate an undefined amount of memory.
void *
malloc2(
	size_t nmemb,
	size_t memb_sz);

#endif
