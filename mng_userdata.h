#ifndef MNG_USERDATA_H
#define MNG_USERDATA_H

#include "types.h"

// zeroes out the structure.
// must be called first on any instance of userdata.
void
mud_init(
	mng_userdata *mudptr);

// makes the userdata usable to libmng callbacks.
// on successful return, the userdata must be cleared.
// if this returns false, the userdata is not usable.
bool
mud_prepare(
	mng_userdata *mudptr,
	const string mng_fname);

// frees any memory allocated to the userdata and clears the structure.
// will safely do nothing if called on an already-cleared or initialized
// structure.
void
mud_clear(
	mng_userdata *mudptr);

// queries the structure to see if it's been prepared.
bool
mud_is_prepared(
	const mng_userdata *mudptr);

#endif
