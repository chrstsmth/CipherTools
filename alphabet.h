#ifndef ALPHABET_H
#define ALPHABET_H

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
	AlphabetSize,
	AlphabetUnknown,
	AlphabetNull
} Alphabet;

Alphabet alphabet_add(Alphabet a, Alphabet b);
Alphabet alphabet_subtract(Alphabet a, Alphabet b);
Alphabet charToAlphabet(char c);
char alphabetToChar(Alphabet a);
void stringToAlphabet(char *c, Alphabet *a);
void alphabetToString(Alphabet *a, char *c);

#endif /* ALPHABET_H */
