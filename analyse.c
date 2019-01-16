#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "analyse.h"
#include "alphabet.h"
#include "language-model.h"

int coincidenceCount(Alphabet *text, int offset);

void frequencyTextSequence(Frequency *freq, Alphabet *text, int n, int step, int offset)
{
	Alphabet *a;

	memset(freq, 0, sizeof(*freq));
	for (a = text + offset; a - text < n; a += step + 1) {
		if (isAlphabetSubsetCipher(*a))
			freq->freq[*a]++;
	}
	for (int a = 0; a < AlphabetSubsetCipher; a++)
		freq->n += freq->freq[a];
}

void frequencyText(Frequency *freq, Alphabet *text, int n)
{
	frequencyTextSequence(freq, text, n, 0, 0);
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

void distributionTextSequence(Distribution *dist, Alphabet *text, int n, int step, int offset)
{
	Frequency freq;
	frequencyTextSequence(&freq, text, n, step, offset);
	distribution(dist, &freq);
}

void distributionText(Distribution *dist, Alphabet *text, int n)
{
	Frequency freq;
	frequencyText(&freq, text, n);
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

double indexOfCoincidence(Distribution *text)
{
	double ic = 0;
	for (int i = 0; i < AlphabetSubsetCipher; i++)
		ic += text->dist[i] * text->dist[i];
	return ic;
}

int coincidenceCount(Alphabet *text, int offset)
{
	Alphabet *a = text; Alphabet *b = text + offset;
	int streak = 0;
	int score = 0;

	for (; *a != AlphabetNull; a++, b++) {
		if (*b == AlphabetNull)
			b = text;
		if (*a == *b) {
			streak++;
		} else {
			if (streak > 1)
				score += streak;
			streak = 0;
		}
	}
	return score;
}

void statistics(Statistics *stats, Distribution *langDist, Distribution *textDist)
{
	stats->chiSquared = chiSquared(textDist, langDist);
	stats->indexOfCoincidence = indexOfCoincidence(textDist);
	stats->measureOfRoughness = measureOfRoughness(textDist);
}

int sequenceInit(Sequence *sequence, Alphabet *text, int n, int step, Distribution *langDist)
{
	sequence->n = step + 1;
	if (!(sequence->off = malloc(sequence->n * sizeof(*sequence->off))))
		return errno;
	memset(sequence->off, 0, sequence->n * sizeof(*sequence->off));

	Statistics *avr = &sequence->avr;
	for (int off = 0; off < sequence->n; off++) {
		TextStatistics *stat = &sequence->off[off];
		distributionTextSequence(&stat->dist, text, n, step, off);
		stat->chiSquared = chiSquared(&stat->dist, langDist);
		stat->indexOfCoincidence = indexOfCoincidence(&stat->dist);
		stat->measureOfRoughness = measureOfRoughness(&stat->dist);

		avr->chiSquared += stat->chiSquared;
		avr->indexOfCoincidence += stat->indexOfCoincidence;
		avr->measureOfRoughness += stat->measureOfRoughness;
	}

	avr->chiSquared /= sequence->n;
	avr->indexOfCoincidence /= sequence->n;
	avr->measureOfRoughness /= sequence->n;

	return 0;
}

int sequencesInit(Sequences *sequences, Alphabet *text, int n, Distribution *langDist)
{
	sequences->n = sequenecMaxStep; //TODO or some multiple of text length
	if (!(sequences->step = malloc(sequences->n * sizeof(*sequences->step))))
		return errno;
	memset(sequences->step, 0, sequences->n * sizeof(*sequences->step));

	for (int step = 0; step < sequences->n; step++) {
		if (sequenceInit(&sequences->step[step], text, n, step, langDist))
			return errno;
	}
	return 0;
}

void textStatistics(TextStatistics *stat, Alphabet *text, int n)
{
		frequencyText(&stat->freq, text, n);
		distribution(&stat->dist, &stat->freq);
		stat->indexOfCoincidence = indexOfCoincidence(&stat->dist);
		stat->measureOfRoughness = measureOfRoughness(&stat->dist);
}

void langStatistics(LangStatistics *stat, LanguageModel *langM)
{
		frequencyLangM(&stat->freq, langM);
		distribution(&stat->dist, &stat->freq);
		stat->indexOfCoincidence = indexOfCoincidence(&stat->dist);
		stat->measureOfRoughness = measureOfRoughness(&stat->dist);
}

int analysis_init(Analysis *a, Alphabet *text, LanguageModel *langM)
{
	memset(a, 0, sizeof(*a));
	a->length = alphabetStrlen(text);
	langStatistics(&a->lang, langM);
	textStatistics(&a->text, text, a->length);
	sequencesInit(&a->seq, text, a->length, &a->lang.dist);
	return 0;
}

void analysis_free()
{
}

void analysis_print(Analysis *a, FILE *f)
{
	fprintf(f, "Distribution\n");
	for (int i = 0; i < AlphabetSubsetCipher; i++) {
			fprintf(f, "%c: %f\n", alphabetToChar(i), a->lang.dist.dist[i]);
	}
	fprintf(f, "\n");

	fprintf(f, "Chi Squared\n");
	for (int i = 0; i < a->seq.n; i++) {
		Sequence *s = &a->seq.step[i];
		fprintf(f, "%f: ", s->avr.chiSquared);
		for (int j = 0; j < s->n; j++) {
			fprintf(f, "%f, ", s->off[j].chiSquared);
		}
		fprintf(f, "\n");
	}
	fprintf(f, "\n");

	fprintf(f, "Index of Coincidence\n");
	for (int i = 0; i < a->seq.n; i++) {
		Sequence *s = &a->seq.step[i];
		fprintf(f, "%f: ", s->avr.indexOfCoincidence);
		for (int j = 0; j < s->n; j++) {
			fprintf(f, "%f, ", s->off[j].indexOfCoincidence);
		}
		fprintf(f, "\n");
	}
	fprintf(f, "\n");

	fprintf(f, "Measure of Roughness\n");
	for (int i = 0; i < a->seq.n; i++) {
		Sequence *s = &a->seq.step[i];
		fprintf(f, "%f: ", s->avr.measureOfRoughness);
		for (int j = 0; j < s->n; j++) {
			fprintf(f, "%f, ", s->off[j].measureOfRoughness);
		}
		fprintf(f, "\n");
	}
	fprintf(f, "\n");
}
