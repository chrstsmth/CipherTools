#ifndef ANALYZE_H
#define ANALYZE_H

#include <stdio.h>

#include "alphabet.h"
#include "language-model.h"

typedef struct Frequency {
	int n;
	int freq[AlphabetSubsetCipher];
} Frequency;

typedef struct Distribution {
	double dist[AlphabetSubsetCipher];
} Distribution;

typedef struct Analysis {
	Frequency lang;
	Frequency text;
	double chiSquared;
	double indexOfCoincidence;
	double measureOfRoughness;
} Analysis;

void frequencyLangM(Frequency *freq, LanguageModel *langM);
void frequencyText(Frequency *freq, Alphabet *text);
double chiSquared(Frequency *lang, Frequency *text);
double measureOfRoughness(Frequency *text);
double indexOfCoincidence(Frequency *text);

void analysis_init(Analysis *a, Alphabet *text, LanguageModel *langM);
void analysis_print(Analysis *a, FILE *f);

#endif
