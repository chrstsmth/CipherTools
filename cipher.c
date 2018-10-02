#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alphabet.h"
#include "candidates.h"
#include "cipher.h"
#include "language-model.h"

int crackUnimplemented(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM)
{
	errno = ENOTSUP;
	return 1;
}

int dictionaryUnimplemented(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary)
{
	errno = ENOTSUP;
	return 1;
}

int serializeKeyAlphabet(Key *key, FILE *f)
{
	char out[alphabetStrlen(key->a) + 1];
	alphabetToString(key->a, out);
	if (fprintf(f, "%s", out) < 0)
		return 1;
	return 0;
}

int dictionaryAttack(const Cipher *cipher, Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary)
{
	char keyString[BUFSIZ];
	int len = alphabetStrlen(cipherText);
	Alphabet plainText[len + 1];
	int line = 1;

	while (fgets(keyString, sizeof(keyString), dictionary) != NULL) {
		char *p = strchr(keyString, '\n');
		if (!p) {
			errno = EOVERFLOW;
			return line;
		}
		*p = '\0';
		Key key;
		if (cipher->initKey(&key, keyString))
			return line;
		if (cipher->decipher(cipherText, plainText, &key))
			return line;
		int score = scoreText(langM, plainText);
		Candidate c = { &key, plainText, cipher, score };
		candidates_copyInsert(candidates, &c);
		line++;
		cipher->freeKey(&key);
	}
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

int caesar_dictionary(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary)
{
	return dictionaryAttack(&ciphers[CipherCaesar], cipherText, candidates, langM, dictionary);
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

int vigenere_dictionary(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary)
{
	return dictionaryAttack(&ciphers[CipherVigenere], cipherText, candidates, langM, dictionary);
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
