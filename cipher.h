#ifndef CIPHER_H
#define CIPHER_H

#include <signal.h>

#include "alphabet.h"
#include "candidates.h"
#include "ciphers.h"
#include "key.h"
#include "language-model.h"

typedef struct Candidates Candidates;

typedef struct CipherInterface {
	int (*encipher)(Alphabet *plainText, Alphabet *cipherText, Key *key);
	int (*decipher)(Alphabet *cipherText, Alphabet *plainText, Key *key);
	int (*dictionary)(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary, sig_atomic_t *exit);
	int (*hillClimb)(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, sig_atomic_t *exit);
	int (*bruteForce)(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, sig_atomic_t *exit);
} CipherInterface;

typedef struct Cipher {
	const char *name;
	const CipherInterface *c;
	const KeyInterface *k;
} Cipher;

int scoreText(LanguageModel *langM, Alphabet* text);
int dictionaryAttack(const Cipher *cipher, Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary, sig_atomic_t *exit);
int hillClimb(const Cipher *cipher, Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, sig_atomic_t *exit);
int bruteForce(const Cipher *cipher, Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, sig_atomic_t *exit);

int dictionaryUnimplemented(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary);
int hillClimbUnimplemented(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, sig_atomic_t *exit);

static char vigenereName[] = "Vigenere";
int vigenere_encipher(Alphabet *plainText, Alphabet *cipherText, Key *key);
int vigenere_decipher(Alphabet *cipherText, Alphabet *plainText, Key *key);
int vigenere_dictionary(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary, sig_atomic_t *exit);
int vigenere_hillClimb(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, sig_atomic_t *exit);
int vigenere_bruteForce(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, sig_atomic_t *exit);

static char caesarName[] = "Caesar";
int caesar_encipher(Alphabet *plainText, Alphabet *cipherText, Key *key);
int caesar_decipher(Alphabet *cipherText, Alphabet *plainText, Key *key);
int caesar_dictionary(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, FILE *dictionary, sig_atomic_t *exit);
int caesar_bruteForce(Alphabet *cipherText, Candidates *candidates, LanguageModel *langM, sig_atomic_t *exit);

static const CipherInterface ciphersInterfaces[] = {
	[CipherCaesar] = {
		&caesar_encipher,
		&caesar_decipher,
		&caesar_dictionary,
		&hillClimbUnimplemented,
		&caesar_bruteForce,
	},
	[CipherVigenere] = {
		&vigenere_encipher,
		&vigenere_decipher,
		&vigenere_dictionary,
		&vigenere_hillClimb,
		&vigenere_bruteForce,
	}
};

static const Cipher ciphers[] = {
	[CipherCaesar] = {
		caesarName,
		&ciphersInterfaces[CipherCaesar],
		&keyInterfaces[CipherCaesar],
	},
	[CipherVigenere] = {
		vigenereName,
		&ciphersInterfaces[CipherVigenere],
		&keyInterfaces[CipherVigenere],
	},
};
#endif
