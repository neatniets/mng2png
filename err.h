#ifndef ERR_H
#define ERR_H

#include <stdio.h>

// print an optionally-formatted message to stderr including diagnostic details
// to find the location the error was triggered in the source.
// the format string and the message string MUST be string literals.
#define ERRF(fmt, ...) \
	fprintf(stderr, "error: "__FILE__":%u: %s: "fmt, __LINE__, __func__, \
		__VA_ARGS__)
#define ERR(msg) \
	ERRF(msg"%s", "")

#endif
