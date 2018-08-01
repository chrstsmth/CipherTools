#include <stdlib.h>
#include <string.h>

#include "alphabet.h"
#include "cipher.h"
#include "language-model.h"

int caesar_encipher(Alphabet *plainText, Alphabet *cipherText, void *key)
{
	Alphabet k[2] = {*(Alphabet*)key, AlphabetNull};
	return vigenere_encipher(plainText, cipherText, k);
}

int caesar_decipher(Alphabet *cipherText, Alphabet *plainText, void *key)
{
	Alphabet k[2] = {*(Alphabet*)key, AlphabetNull};
	return vigenere_decipher(cipherText, plainText, k);
}

int caesar_keySize(char *argv)
{
	return sizeof(Alphabet);
}

int caesar_parseKey(char *argv, void *key)
{
	Alphabet *k = (Alphabet*)key;
	*k = charToAlphabet(*argv);
	return 0; /* TODO */
}

int vigenere_encipher(Alphabet *plainText, Alphabet *cipherText, void *key)
{
	Alphabet *k = (Alphabet*)key;
	Alphabet *k0 = k;

	if (*k == AlphabetNull)
		return 1;

	for(; *plainText != AlphabetNull; plainText++, cipherText++, k++) {
		if (*k == AlphabetNull)
			k = k0;
		*cipherText = alphabet_add(*plainText, *k);
	}
	*cipherText = AlphabetNull;
	return 0;
}

int vigenere_decipher(Alphabet *cipherText, Alphabet *plainText, void *key)
{
	Alphabet *k = (Alphabet*)key;
	Alphabet *k0 = k;

	if (*k == AlphabetNull)
		return 1;

	for(; *cipherText != AlphabetNull; plainText++, cipherText++, k++) {
		if (*k == AlphabetNull)
			k = k0;
		*plainText = alphabet_subtract(*cipherText, *k);
	}
	*plainText = AlphabetNull;
	return 0;
}

int vigenere_keySize(char *argv)
{
	return sizeof(Alphabet) * (strlen(argv) + 1);
}

int vigenere_parseKey(char *argv, void *key)
{
	stringToAlphabet(argv, key);
	return 0; /* TODO */
}
