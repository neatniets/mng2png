#ifndef VERBOSE_H
#define VERBOSE_H

#include <stdbool.h>

extern bool is_verbose;

// print an informational (non-error) message to stdout, if the verbosity flag
// is set.
void
verb(
	const char *msg);
void
verbf(
	const char *fmt,
	...);

#endif
