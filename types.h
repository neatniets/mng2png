#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <libmng.h>
#include <stdbool.h>

typedef char* string;

// this is the structure passed as a pointer to the libmng library for use in
// callback functions.
// make sure its memory location doesn't change!
typedef struct mng_userdata mng_userdata;
struct mng_userdata {
	FILE *fileptr;
};

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#endif
