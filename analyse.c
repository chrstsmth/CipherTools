#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "analyse.h"
#include "alphabet.h"
#include "language-model.h"

void frequencyText(Frequency *freq, Alphabet *text)
{
	Alphabet *a;
	int n = alphabetStrlen(text); /* TODO send len, or store it with text */

	memset(freq, 0, sizeof(*freq));
	for (a = text; a - text < n; a++) {
		freq->freq[*a]++;
	}
	freq->n = a - text;
}

void frequencyLangM(Frequency *freq, LanguageModel *langM)
{
	freq->n = langM->head->freq;
	for (Alphabet a = 0; a < (int)AlphabetSubsetCipher; a++) {
		Node *next = langM->head->next[a];
		freq->freq[a] = 0;
		if (next)
			freq->freq[a] = next->freq;
	}
}

double chiSquared(Frequency *text, Frequency *lang)
{
	double chiSquared = 0;
	for (Alphabet a = 0; a < (int)AlphabetSubsetCipher; a++) {
		double c = text->freq[a] / (double)text->n;
		double e = lang->freq[a] / (double)lang->n;
		double ce =  c - e;
		chiSquared += ce * ce / e;
	}
	return chiSquared;
}

double measureOfRoughness(Frequency *text)
{
	double measureOfRoughness = 0;
	for (Alphabet a = 0; a < (int)AlphabetSubsetCipher; a++) {
		double prob = text->freq[a] / (double) text->n;
		measureOfRoughness += prob * prob;
	}
	measureOfRoughness -= 2.0 / AlphabetSubsetCipher;
	measureOfRoughness += 1.0 / AlphabetSubsetCipher;
	return measureOfRoughness;
}

double indexOfCoincidence(Frequency *text)
{
	/* Compute incrementally to avoid overflow for large frequencies */
	double ic = 0;
	double n = text->n;
	for (Alphabet a = 0; a < (int)AlphabetSubsetCipher; a++) {
		double freq = text->freq[a];
		double a = freq / n;
		double b = (freq - 1) / (n - 1);
		ic += a * b;
	}
	return ic;
}

void analysis_init(Analysis *a, Alphabet *text, LanguageModel *langM)
{
	memset(a, 0, sizeof(*a));
	frequencyLangM(&a->lang, langM);
	frequencyText(&a->text, text);
	a->chiSquared = chiSquared(&a->text, &a->lang);
	a->indexOfCoincidence = indexOfCoincidence(&a->lang);
	a->measureOfRoughness = measureOfRoughness(&a->lang);
}

void analysis_print(Analysis *a, FILE *f)
{
	fprintf(f, "Total: %d %d\n", a->text.n, a->lang.n);
	for (Alphabet i = 0; i < (int)AlphabetSubsetCipher; i++) {
		int textFreq = a->text.freq[i];
		double textPercent = textFreq / (double)a->text.n * 100;
		int langFreq = a->lang.freq[i];
		double langPercent = langFreq  / (double)a->lang.n * 100;
		fprintf(f, "%c: %d %f %d %f\n", alphabetToChar(i), textFreq, textPercent, langFreq, langPercent);
	}
}
