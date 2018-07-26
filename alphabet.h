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

Alphabet charToAlphabet(char c);
char alphabetToChar(Alphabet a);

#endif /* ALPHABET_H */
