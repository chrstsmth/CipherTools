#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alphabet.h"
#include "cipher.h"
#include "language-model.h"

int crackUnimplemented(Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM)
{
	errno = ENOTSUP;
	return 1;
}

int dictionaryUnimplemented(Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM, FILE *dictionary)
{
	errno = ENOTSUP;
	return 1;
}

void freeKey(Key *key)
{
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

int dictionaryAttack(Cipher cipher, Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM, FILE *dictionary)
{
	char keyString[BUFSIZ];
	int len = alphabetStrlen(cipherText);
	Alphabet buffer[2][len + 1];
	int select = 0;
	int score = 0;
	int line = 1;

	while (fgets(keyString, sizeof(keyString), dictionary) != NULL) {
		char *p = strchr(keyString, '\n');
		if (!p) {
			errno = EOVERFLOW;
			return line;
		}
		*p = '\0';
		Key key;
		if (cipher.initKey(&key, keyString))
			return line;
		if (cipher.decipher(cipherText, buffer[select], &key))
			return line;
		int newScore = scoreText(langM, buffer[select]);
		if (newScore > score) {
			score = newScore;
			select = (select + 1) % 2;
		}
		line++;
		cipher.freeKey(&key);
	}
	select = (select + 1) % 2;
	memcpy(plainText, buffer[select], len * sizeof(Alphabet));
	plainText[len] = AlphabetNull;
	return 0;
}

int caesar_encipher(Alphabet *plainText, Alphabet *cipherText, Key *key)
{
	return vigenere_encipher(plainText, cipherText, key);
}

int caesar_decipher(Alphabet *cipherText, Alphabet *plainText, Key *key)
{
	return vigenere_decipher(cipherText, plainText, key);
}

int caesar_crack(Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM)
{
	Alphabet a[2] = {a[1] = AlphabetNull};
	Key key = {.a = a};
	int score = 0;
	int len = alphabetStrlen(cipherText);
	Alphabet buffer[2][len + 1];
	int select = 0;

	for (int i = 0; i < AlphabetSubsetCipher; i++) {
		key.a[0] = (Alphabet)i;
		caesar_decipher(cipherText, buffer[select], &key);

		int newScore = scoreText(langM, buffer[select]);
		if (newScore > score) {
			score = newScore;
			select = (select + 1) % 2;
		}
	}
	select = (select + 1) % 2;
	memcpy(plainText, buffer[select], len * sizeof(Alphabet));
	plainText[len] = AlphabetNull;
	return 0;
}

int caesar_dictionary(Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM, FILE *dictionary)
{
	return dictionaryAttack(ciphers[CipherCaesar], cipherText, plainText, langM, dictionary);
}

int caesar_keySize(char *argv)
{
	return sizeof(Alphabet);
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

int vigenere_encipher(Alphabet *plainText, Alphabet *cipherText, Key *key)
{
	Alphabet *k = key->a;
	Alphabet *k0 = k;

	if (*k == AlphabetNull)
		return 0;

	for(; *plainText != AlphabetNull; plainText++, cipherText++, k++) {
		if (*k == AlphabetNull)
			k = k0;
		*cipherText = alphabet_add(*plainText, *k);
	}
	*cipherText = AlphabetNull;
	return 0;
}

int vigenere_decipher(Alphabet *cipherText, Alphabet *plainText, Key *key)
{
	Alphabet *k = key->a;
	Alphabet *k0 = k;

	if (*k == AlphabetNull)
		return 0;

	for(; *cipherText != AlphabetNull; plainText++, cipherText++, k++) {
		if (*k == AlphabetNull)
			k = k0;
		*plainText = alphabet_subtract(*cipherText, *k);
	}
	*plainText = AlphabetNull;
	return 0;
}

int vigenere_dictionary(Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM, FILE *dictionary)
{
	return dictionaryAttack(ciphers[CipherVigenere], cipherText, plainText, langM, dictionary);
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

int scoreText(LanguageModel *langM, Alphabet* text)
{
	int score = 0;
	for (;*text != AlphabetNull; text++) {
		Node* cursor = langM->head;
		for (int i = 0; cursor && text[i] != AlphabetNull; i++) {
			Alphabet a = text[i];
			score+=cursor->freq*i;
			cursor = cursor->next[a];
		}
	}
	return score;
}
