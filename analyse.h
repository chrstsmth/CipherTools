#ifndef ANALYZE_H
#define ANALYZE_H

#include "alphabet.h"
#include "language-model.h"

typedef struct Frequency {
	int n;
	int freq[AlphabetSubsetCipher];
} Frequency;

void frequencyLangM(LanguageModel *langM, Frequency *freq);
void frequencyTextSkip(Alphabet *text, Frequency *freq, int skip, int offset);
void frequencyText(Alphabet *text, Frequency *freq);
double chiSquared(Frequency *text, Frequency *lang);
double measureOfRoughness(Frequency *text);
double indexOfCoincidence(Frequency *text);

#endif
