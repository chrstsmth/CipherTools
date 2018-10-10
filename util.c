#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "util.h"

void die(const char *errstr, ...)
{
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}
