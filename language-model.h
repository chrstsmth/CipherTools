#ifndef LANGUAGE_MODEL_H
#define LANGUAGE_MODEL_H

#include <stdio.h>

typedef struct Node Node;

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

struct Node {
	Node *next[AlphabetSize];
	int freq;
};

typedef struct {
	Node *head;
	int depth;
} LanguageModel;

int langM_init(LanguageModel *langM);
void langM_free(LanguageModel *langM);
int langM_insertWord(LanguageModel *langM, char *c);
int langM_insertFile(LanguageModel *langM, int depth, FILE * in);
int langM_deserialize(LanguageModel *langM, FILE *f);
int langM_serialize(LanguageModel *langM, FILE *f);
Alphabet charToAlphabet(char c);
char alphabetToChar(Alphabet a);

#endif /* LANGUAGE_MODEL_H */
