#include <stdarg.h>
#include <stdio.h>

#include "verbose.h"

bool is_verbose = true; // TODO: change default to false.

void
verb(
	const char *msg)
{
	verbf(msg);
}
void
verbf(
	const char *fmt,
	...)
{
	if (!is_verbose) {
		return;
	}

	fputs("info: ", stdout);
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}
