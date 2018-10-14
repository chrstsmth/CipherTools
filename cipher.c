#include <errno.h>
#include <signal.h>
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

int hillClimbUnimplemented(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, sig_atomic_t *exit)
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

int bruteForce(const Cipher *cipher, Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, sig_atomic_t *exit)
{
	int len = alphabetStrlen(cipherText);
	Alphabet plainText[len + 1];
	Key key;

	cipher->k->initFirstKey(&key);
	while (!*exit) {
		if (cipher->c->decipher(cipherText, plainText, &key))
			return 1;
		int score = scoreText(langM, plainText);
		Candidate c = { &key, plainText, cipher, score };
		candidates_copyInsert(candidates, &c);
		if (cipher->k->nextKey(&key))
			break;
	}
	return 0;
}

int dictionaryAttack(const Cipher *cipher, Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary, sig_atomic_t *exit)
{
	char keyString[BUFSIZ];
	int len = alphabetStrlen(cipherText);
	Alphabet plainText[len + 1];
	int line = 1;

	while ((fgets(keyString, sizeof(keyString), dictionary) != NULL) && !*exit) {
		char *p = strchr(keyString, '\n');
		if (!p) {
			errno = EOVERFLOW;
			return line;
		}
		*p = '\0';
		Key key;
		if (cipher->k->initKey(&key, keyString))
			return line;
		if (cipher->c->decipher(cipherText, plainText, &key))
			return line;
		int score = scoreText(langM, plainText);
		Candidate c = { &key, plainText, cipher, score };
		candidates_copyInsert(candidates, &c);
		line++;
		cipher->k->freeKey(&key);
	}
	return 0;
}

int hillClimb(const Cipher *cipher, Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, sig_atomic_t *exit)
{
	Alphabet plainText[alphabetStrlen(cipherText) + 1];

	while (!*exit) {
		Key climberKey;
		cipher->k->initRandomKey(&climberKey);
		cipher->c->decipher(cipherText, plainText, &climberKey);
		int score = scoreText(langM, plainText);

		Candidate c = { &climberKey, plainText, cipher, score };
		candidates_copyInsert(candidates, &c);

		/* Hill climb until maxima is reached */
		bool maxima;
		do {
			maxima = true;

			/* Find best mutation */
			int mutation = 0;
			Key mutatedKey;
			Key baseKey;
			cipher->k->copyKey(&baseKey, &climberKey);
			cipher->k->copyKey(&mutatedKey, &climberKey);
			while (!cipher->k->nextMutationKey(&mutatedKey, &mutation)) {
				cipher->c->decipher(cipherText, plainText, &mutatedKey);
				int mutatedScore = scoreText(langM, plainText);

				if (mutatedScore > score) {
					score = mutatedScore;
					cipher->k->freeKey(&climberKey);
					cipher->k->copyKey(&climberKey, &mutatedKey);
					maxima = false;

					Candidate c = { &mutatedKey, plainText, cipher, mutatedScore };
					candidates_copyInsert(candidates, &c);
				}
			}
			cipher->k->freeKey(&baseKey);
			cipher->k->freeKey(&mutatedKey);
		} while (!maxima && !*exit);
		cipher->k->freeKey(&climberKey);
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

int caesar_dictionary(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary, sig_atomic_t *exit)
{
	return dictionaryAttack(&ciphers[CipherCaesar], cipherText, candidates, langM, dictionary, exit);
}

int caesar_bruteForce(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, sig_atomic_t *exit)
{
	return bruteForce(&ciphers[CipherCaesar], cipherText, candidates, langM, exit);
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

int vigenere_dictionary(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary, sig_atomic_t *exit)
{
	return dictionaryAttack(&ciphers[CipherVigenere], cipherText, candidates, langM, dictionary, exit);
}

int vigenere_hillClimb(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, sig_atomic_t *exit)
{
	return hillClimb(&ciphers[CipherVigenere], cipherText, candidates, langM, exit);
}

int vigenere_bruteForce(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, sig_atomic_t *exit)
{
	return bruteForce(&ciphers[CipherVigenere], cipherText, candidates, langM, exit);
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
