#ifndef ANALYZE_H
#define ANALYZE_H

#include <stdio.h>

#include "alphabet.h"
#include "language-model.h"

#define sequenecMaxStep 20

typedef struct Frequency {
	int n;
	int freq[AlphabetSubsetCipher];
} Frequency;

typedef struct Distribution {
	double dist[AlphabetSubsetCipher];
} Distribution;

/*
typedef struct Order {
	double Alphabet[AlphabetSubsetCipher];
} Order;
*/

typedef struct TextStatistics {
	Frequency freq;
	Distribution dist;
	double chiSquared;
	double indexOfCoincidence;
	double measureOfRoughness;
} TextStatistics;

typedef struct LangStatistics {
	Frequency freq;
	Distribution dist;
	double indexOfCoincidence;
	double measureOfRoughness;
} LangStatistics;

typedef struct Statistics {
	double chiSquared;
	double indexOfCoincidence;
	double measureOfRoughness;
} Statistics ;

typedef struct Sequence {
	int n;
	TextStatistics *off;
	Statistics avr;
} Sequence;

typedef struct Sequences {
	int n;
	Sequence *step;
} Sequences;

typedef struct Coincidence {
	int n;
	//TODO
} Coincidence;

typedef struct Analysis {
	int length;
	LangStatistics lang;
	TextStatistics text;
	Sequences seq;
} Analysis;

void frequencyLangM(Frequency *freq, LanguageModel *langM);
void frequencyText(Frequency *freq, Alphabet *text, int n);

void distribution(Distribution *dist, Frequency *freq);
void distributionLangM(Distribution *dist, LanguageModel *langM);
void distributionText(Distribution *dist, Alphabet *text, int n);

double chiSquared(Distribution *lang, Distribution *text);
double measureOfRoughness(Distribution *text);
double indexOfCoincidence(Distribution *text);
double coincidenceCounting(Frequency *text);

int analysis_init(Analysis *a, Alphabet *text, LanguageModel *langM);
void analysis_free();
void analysis_print(Analysis *a, FILE *f);

double kasaskiScore(Alphabet *text, int keyLen);

#endif
