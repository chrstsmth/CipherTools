#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "alphabet.h"
#include "key.h"

void freeKey(Key *key)
{
	if (!key)
		return;
	free(key->buf);
}

int copyKey(Key *key, Key *other)
{
	key->n = other->n;
	if (!(key->buf = malloc(key->n)))
		return 1;
	memcpy(key->buf, other->buf, key->n);
	return 0;
}

int equalKey(Key *key, Key *other)
{
	if (key->n != other->n)
		return 0;
	return !(memcmp(key->buf, other->buf, key->n));
}

int caesar_initKey(Key *key, char *argv)
{
	if (strlen(argv) != 1) {
		errno = EINVAL;
		return 1;
	}

	key->n = sizeof(Alphabet) * 2;

	if (!(key->buf = malloc(key->n)))
		return 1;
	key->a[0] = charToAlphabet(*argv);
	key->a[1] = AlphabetNull;

	if (!isAlphabetSubsetCipher(key->a[1]))
		return 1;
	return 0;
}

int vigenere_initKey(Key *key, char *argv)
{
	int len = strlen(argv);
	if (!len) {
		errno = EINVAL;
		return 1;
	}

	key->n = sizeof(Alphabet) * (len + 1);

	if (!(key->buf = malloc(key->n)))
			return 1;

	if (!(stringToAlphabet(argv, key->a) == AlphabetSubsetCipher)) {
		errno = EINVAL;
		return 1;
	}
	return 0;
}
