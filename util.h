#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#include "alphabet.h"

void die(const char *errstr, ...);
int text_init(Alphabet **text, FILE *f);

#endif
