#ifndef CIPHER_H
#define CIPHER_H

#include "alphabet.h"
#include "candidates.h"
#include "key.h"
#include "language-model.h"

typedef struct Candidates Candidates;

enum {
	CipherCaesar,
	CipherVigenere,
};

typedef struct {
	const char *name;
	int (*encipher)(Alphabet *plainText, Alphabet *cipherText, Key *key);
	int (*decipher)(Alphabet *cipherText, Alphabet *plainText, Key *key);
	int (*crack)(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM);
	int (*dictionary)(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary);
	int (*initKey)(Key *key, char *argv);
	int (*serializeKey)(Key *key, FILE *f);
	int (*copyKey)(Key *key, Key *other);
	void (*freeKey)(Key *key);
} Cipher;

int scoreText(LanguageModel *langM, Alphabet* text);
int dictionaryAttack(const Cipher *cipher, Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary);

int crackUnimplemented(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM);
int dictionaryUnimplemented(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary);

static char vigenereName[] = "Vigenere";
int vigenere_encipher(Alphabet *plainText, Alphabet *cipherText, Key *key);
int vigenere_decipher(Alphabet *cipherText, Alphabet *plainText, Key *key);
int vigenere_dictionary(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary);

static char caesarName[] = "Caesar";
int caesar_encipher(Alphabet *plainText, Alphabet *cipherText, Key *key);
int caesar_decipher(Alphabet *cipherText, Alphabet *plainText, Key *key);
int caesar_dictionary(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary);

static const Cipher ciphers[] = {
	[CipherCaesar] = {
		caesarName,
		&caesar_encipher,
		&caesar_decipher,
		&crackUnimplemented,
		&caesar_dictionary,
		&caesar_initKey,
		&serializeKeyAlphabet,
		&copyKey,
		&freeKey,
	},
	[CipherVigenere] = {
		vigenereName,
		&vigenere_encipher,
		&vigenere_decipher,
		&crackUnimplemented,
		&vigenere_dictionary,
		&vigenere_initKey,
		&serializeKeyAlphabet,
		&copyKey,
		&freeKey,
	}
};
#endif
