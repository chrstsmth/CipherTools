#include "alphabet.h"
#include "language-model.h"

#ifndef CIPHER_H
#define CIPHER_H

enum {
	CipherCaesar,
	CipherVigenere,
};

typedef struct {
	union {
		void *buf;
		Alphabet *a;
	};
	size_t n;
} Key;

typedef struct {
	int (*encipher)(Alphabet *plainText, Alphabet *cipherText, Key *key);
	int (*decipher)(Alphabet *cipherText, Alphabet *plainText, Key *key);
	int (*crack)(Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM);
	int (*dictionary)(Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM, FILE *dictionary);
	int (*initKey)(Key *key, char *argv);
	int (*copyKey)(Key *key, Key *other);
	void (*freeKey)(Key *key);
} Cipher;

int scoreText(LanguageModel *langM, Alphabet* text);
int dictionaryAttack(Cipher cipher, Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM, FILE *dictionary);

int crackUnimplemented(Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM);
int dictionaryUnimplemented(Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM, FILE *dictionary);
void freeKey(Key *key);
int copyKey(Key *key, Key *other);

int vigenere_encipher(Alphabet *plainText, Alphabet *cipherText, Key *key);
int vigenere_decipher(Alphabet *cipherText, Alphabet *plainText, Key *key);
int vigenere_dictionary(Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM, FILE *dictionary);
int vigenere_initKey(Key *key, char *argv);

int caesar_encipher(Alphabet *plainText, Alphabet *cipherText, Key *key);
int caesar_decipher(Alphabet *cipherText, Alphabet *plainText, Key *key);
int caesar_crack(Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM);
int caesar_dictionary(Alphabet *cipherText, Alphabet *plainText, LanguageModel *langM, FILE *dictionary);
int caesar_initKey(Key *key, char *argv);

static const Cipher ciphers[] = {
	[CipherCaesar] = {
		&caesar_encipher,
		&caesar_decipher,
		&caesar_crack,
		&caesar_dictionary,
		&caesar_initKey,
		&copyKey,
		&freeKey,
	},
	[CipherVigenere] = {
		&vigenere_encipher,
		&vigenere_decipher,
		&crackUnimplemented,
		&vigenere_dictionary,
		&vigenere_initKey,
		&copyKey,
		&freeKey,
	}
};
#endif /* CIPHER_H */
