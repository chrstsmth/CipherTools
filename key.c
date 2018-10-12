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

int caesar_initFirstKey(Key *key)
{
	char *s = "a";
	return caesar_initKey(key, s);
}

int caesar_nextKey(Key *key)
{
	/* Check if last key */
	if (key->a[0] == AlphabetSubsetCipher - 1)
		return 1;
	key->a[0]++;
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

int vigenere_initFirstKey(Key *key)
{
	char *s = "a";
	return vigenere_initKey(key, s);
}

int vigenere_initRandomKey(Key *key)
{
	int len = rand() % 10 + 1;
	key->n = sizeof(Alphabet) * (len + 1);
	if (!(key->buf = malloc(key->n)))
		return 1;
	key->a[len] = AlphabetNull;
	for (Alphabet *a = key->a; *a != AlphabetNull; a++)
		*a = (rand() % AlphabetSubsetCipher);
	return 0;
}

int vigenere_nextMutationKey(Key *key, int *m)
{
	int prev = *m;
	int next = *m + 1;
	int len = key->n / sizeof(Alphabet) - 1;
	int index, offset;

	/* undo previous mutation */
	index = prev / (AlphabetSubsetCipher - 1);
	offset = prev % (AlphabetSubsetCipher - 1);
	key->a[index] = alphabet_subtract(key->a[index], offset);

	/* do next mutation */
	index = next / (AlphabetSubsetCipher - 1);
	offset = next % (AlphabetSubsetCipher - 1);
	if (index >= len)
		return 1;
	key->a[index] = alphabet_add(key->a[index], offset);

	*m = next;
	return 0;
}

int vigenere_nextKey(Key *key)
{
	Alphabet *a;

	for (a = key->a; *a == AlphabetSubsetCipher - 1; a++)
		*a = 0;
	if (*a == AlphabetNull) {
		key->n += sizeof(Alphabet);
		if (!(key->buf = realloc(key->buf, key->n)))
			return 1;
		int len = key->n / sizeof(Alphabet);
		key->a[len - 2] = (Alphabet)0;
		key->a[len - 1] = AlphabetNull;
	} else {
		(*a)++;
	}
	return 0;
}
