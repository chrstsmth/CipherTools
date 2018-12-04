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
	Distribution langDist;
	Distribution textDist;
	double chiSquared;
	double indexOfCoincidence;
	double measureOfRoughness;
} Analysis;

void frequencyLangM(Frequency *freq, LanguageModel *langM);
void frequencyText(Frequency *freq, Alphabet *text);

void distribution(Distribution *dist, Frequency *freq);
void distributionLangM(Distribution *dist, LanguageModel *langM);
void distributionText(Distribution *dist, Alphabet *text);

double chiSquared(Distribution *lang, Distribution *text);
double measureOfRoughness(Distribution *text);
double indexOfCoincidence(Distribution *text);

void analysis_init(Analysis *a, Alphabet *text, LanguageModel *langM);
void analysis_print(Analysis *a, FILE *f);

#endif
