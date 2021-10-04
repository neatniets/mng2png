#include <stdio.h>

#include "types.h"
#include "mng_userdata.h"

void
mud_init(
	mng_userdata *mudptr)
{
	mudptr->fileptr = NULL;
}

bool
mud_prepare(
	mng_userdata *mudptr,
	const string mng_fname)
{
	mud_clear(mudptr); // previous structure will be lost anyway.
	// this program will only be reading mng and not writing any, so it's
	// opened readonly.
	mudptr->fileptr = fopen(mng_fname, "r");
	bool is_valid = !(mudptr->fileptr == NULL);
	if (!is_valid) {
		fprintf(stderr,
			"mud_prepare: fopen(%s, r) failed.\n",
			mng_fname);
	}
	return is_valid;
}

void
mud_clear(
	mng_userdata *mudptr)
{
	if (mudptr->fileptr) {
		fclose(mudptr->fileptr);
	}
	mud_init(mudptr);
}

bool
mud_is_prepared(
	const mng_userdata *mudptr)
{
	return !(mudptr->fileptr);
}
