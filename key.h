#ifndef KEY_H
#define KEY_H

#include <stdlib.h>
#include <stdio.h>

#include "alphabet.h"

typedef struct Key {
	union {
		void *buf;
		Alphabet *a;
	};
	size_t n;
} Key;

typedef struct KeyInterface {
	int (*initKey)(Key *key, char *argv);
	int (*serializeKey)(Key *key, FILE *f);
	int (*copyKey)(Key *key, Key *other);
	void (*freeKey)(Key *key);
} KeyInterface;

void freeKey(Key *key);
int serializeKeyAlphabet(Key *key, FILE *f);
int copyKey(Key *key, Key *other);

int caesar_initKey(Key *key, char *argv);
int vigenere_initKey(Key *key, char *argv);

#endif
