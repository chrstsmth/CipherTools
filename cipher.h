#include "alphabet.h"
#include "language-model.h"

enum {
	CipherCaesar,
	CipherVigenere,
};

typedef struct {
	int (*encipher)(Alphabet *plainText, Alphabet *cipherText, void *key);
	int (*decipher)(Alphabet *cipherText, Alphabet *plainText, void *key);
	int (*keySize)(char *argv);
	int (*parseKey)(char *argv, void *key);
} Cipher;

int scoreText(LanguageModel *langM, Alphabet* text);

int vigenere_encipher(Alphabet *plainText, Alphabet *cipherText, void *key);
int vigenere_decipher(Alphabet *cipherText, Alphabet *plainText, void *key);
int vigenere_keySize(char *argv);
int vigenere_parseKey(char *argv, void *key);

int caesar_encipher(Alphabet *plainText, Alphabet *cipherText, void *key);
int caesar_decipher(Alphabet *cipherText, Alphabet *plainText, void *key);
int caesar_keySize(char *argv);
int caesar_parseKey(char *argv, void *key);

static const Cipher ciphers[] = {
	[CipherCaesar] = {
		&caesar_encipher,
		&caesar_decipher,
		&caesar_keySize,
		&caesar_parseKey,
	},
	[CipherVigenere] = {
		&vigenere_encipher,
		&vigenere_decipher,
		&vigenere_keySize,
		&vigenere_parseKey,
	}
};
