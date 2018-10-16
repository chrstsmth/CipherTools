#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "alphabet.h"
#include "util.h"

void die(const char *errstr, ...)
{
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

int text_init(Alphabet **text, FILE *f)
{
	int len;
	/* deterine file length */
	if (fseek(f, 0, SEEK_END))
		return -1;
	if ((len = ftell(f)) < 0)
		return -1;
	if (fseek(f, 0, SEEK_SET))
		return -1;

	char buff[len + 1];
	int read = fread(buff, sizeof(*buff), len, f);
	buff[read] = '\0';
	if (ferror(f))
		return -1;

	if (!(*text = malloc((read + 1) * sizeof(**text))))
		return -1;
	if (stringToAlphabet(buff, *text) > AlphabetSubsetLangM) {
		errno = EINVAL;
		return -1;
	}
	return len;
}
