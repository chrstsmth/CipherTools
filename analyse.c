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
		if (isAlphabetSubsetCipher(*a))
			freq->freq[*a]++;
	}
	for (int a = 0; a < AlphabetSubsetCipher; a++)
		freq->n += freq->freq[a];
}

void frequencyLangM(Frequency *freq, LanguageModel *langM)
{
	memset(freq, 0, sizeof(*freq));
	for (Alphabet a = 0; a < (int)AlphabetSubsetCipher; a++) {
		Node *next = langM->head->next[a];
		freq->freq[a] = 0;
		if (next) {
			freq->freq[a] = next->freq;
			freq->n += next->freq;
		}
	}
}

void distributionText(Distribution *dist, Alphabet *text)
{
	Frequency freq;
	frequencyText(&freq, text);
	distribution(dist, &freq);
}

void distributionLangM(Distribution *dist, LanguageModel *langM)
{
	Frequency freq;
	frequencyLangM(&freq, langM);
	distribution(dist, &freq);
}

void distribution(Distribution *dist, Frequency *freq)
{
	for (Alphabet a = 0; a < (int)AlphabetSubsetCipher; a++)
		dist->dist[a] = freq->freq[a] / (double)freq->n;
}

double chiSquared(Distribution *text, Distribution *lang)
{
	double chiSquared = 0;
	for (Alphabet a = 0; a < (int)AlphabetSubsetCipher; a++) {
		double c = text->dist[a];
		double e = lang->dist[a];
		double ce =  c - e;
		chiSquared += ce * ce / e;
	}
	return chiSquared;
}

double measureOfRoughness(Distribution *text)
{
	double measureOfRoughness = 0;
	for (Alphabet a = 0; a < (int)AlphabetSubsetCipher; a++) {
		double prob = text->dist[a];
		measureOfRoughness += prob * prob;
	}
	measureOfRoughness -= 2.0 / AlphabetSubsetCipher;
	measureOfRoughness += 1.0 / AlphabetSubsetCipher;
	return measureOfRoughness;
}

/* indexOfCoincidence is the probability that two letters chosen at random
* from the given cipher text are alike.  */
double indexOfCoincidence(Distribution *text)
{
	double ic = 0;
	for (int i = 0; i < AlphabetSubsetCipher; i++)
		ic += text->dist[i] * text->dist[i];
	return ic;
}

void analysis_init(Analysis *a, Alphabet *text, LanguageModel *langM)
{
	memset(a, 0, sizeof(*a));
	distributionLangM(&a->langDist, langM);
	distributionText(&a->textDist, text);
	a->chiSquared = chiSquared(&a->textDist, &a->langDist);
	a->indexOfCoincidence = indexOfCoincidence(&a->langDist);
	a->measureOfRoughness = measureOfRoughness(&a->langDist);
}

void analysis_print(Analysis *a, FILE *f)
{
	for (Alphabet i = 0; i < (int)AlphabetSubsetCipher; i++) {
		fprintf(f, "%c: %f %f\n", alphabetToChar(i), a->textDist.dist[i], a->langDist.dist[i]);
	}
	fprintf(f, "chi: %f \n", a->chiSquared);
	fprintf(f, "roughness: %f \n", a->measureOfRoughness);
	fprintf(f, "IC: %f \n", a->indexOfCoincidence);
}
