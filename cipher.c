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
		void *key;
		if (!(key = malloc(cipher.keySize(keyString))))
			return line;
		if (cipher.parseKey(keyString, key))
			return line;
		if (cipher.decipher(cipherText, buffer[select], key))
			return line;
		int newScore = scoreText(langM, buffer[select]);
		if (newScore > score) {
			score = newScore;
			select = (select + 1) % 2;
		}
		line++;
		free(key);
	}
	select = (select + 1) % 2;
	memcpy(plainText, buffer[select], len * sizeof(Alphabet));
	plainText[len] = AlphabetNull;
	return 0;
}

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

int caesar_crack(Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM)
{
	Alphabet key;
	int score = 0;
	int len = alphabetStrlen(cipherText);
	Alphabet buffer[2][len + 1];
	int select = 0;

	for (int i = 0; i < AlphabetSubsetCipher; i++) {
		key = (Alphabet)i;
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

int caesar_parseKey(char *argv, void *key)
{
	if (strlen(argv) != 1) {
		errno = EINVAL;
		return 1;
	}
	Alphabet *k = (Alphabet*)key;
	*k = charToAlphabet(*argv);
	return (isAlphabetSubsetCipher(*k)) ? 0 : 1;
}

int vigenere_encipher(Alphabet *plainText, Alphabet *cipherText, void *key)
{
	Alphabet *k = (Alphabet*)key;
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

int vigenere_decipher(Alphabet *cipherText, Alphabet *plainText, void *key)
{
	Alphabet *k = (Alphabet*)key;
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

int vigenere_keySize(char *argv)
{
	return sizeof(Alphabet) * (strlen(argv) + 1);
}

int vigenere_parseKey(char *argv, void *key)
{
	Alphabet *k = (Alphabet*)key;
	if (!strlen(argv)) {
		errno = EINVAL;
		return 1;
	}
	if (!(stringToAlphabet(argv, k) == AlphabetSubsetCipher)) {
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
