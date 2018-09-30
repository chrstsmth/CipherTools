#ifndef ALPHABET_H
#define ALPHABET_H

#include <stdbool.h>

typedef enum {
	AlphabetA,
	AlphabetB,
	AlphabetC,
	AlphabetD,
	AlphabetE,
	AlphabetF,
	AlphabetG,
	AlphabetH,
	AlphabetI,
	AlphabetJ,
	AlphabetK,
	AlphabetL,
	AlphabetM,
	AlphabetN,
	AlphabetO,
	AlphabetP,
	AlphabetQ,
	AlphabetR,
	AlphabetS,
	AlphabetT,
	AlphabetU,
	AlphabetV,
	AlphabetW,
	AlphabetX,
	AlphabetY,
	AlphabetZ,
	AlphabetSpace,
	AlphabetPeriod,
	AlphabetUnknown,
	AlphabetNull,
} Alphabet;

typedef enum {
	AlphabetSubsetCipher = (AlphabetZ + 1),
	AlphabetSubsetLangM  = (AlphabetPeriod + 1),
	AlphabetSuperset     = (AlphabetUnknown + 1),
} AlphabetSubset;

Alphabet alphabet_add(Alphabet a, Alphabet b);
Alphabet alphabet_subtract(Alphabet a, Alphabet b);
Alphabet charToAlphabet(char c);
char alphabetToChar(Alphabet a);
AlphabetSubset stringToAlphabet(char *c, Alphabet *a);
void alphabetToString(Alphabet *a, char *c);
bool isAlphabetSubsetCipher(Alphabet a);
bool isAlphabetSubsetLangM(Alphabet a);
bool isAlphabetSubsetAll(Alphabet a);
AlphabetSubset getAlphabetSubset(Alphabet a);

int alphabetStrlen(Alphabet *a);

#endif
