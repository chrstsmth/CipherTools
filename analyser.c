#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alphabet.h"
#include "analyse.h"
#include "arg.h"
#include "language-model.h"
#include "util.h"

typedef struct Options {
	LanguageModel langM;
	Alphabet *text;
	int textLength;
} Options;

typedef struct Analysis {
	Frequency lang;
	Frequency text;
	double chiSquared;
	double indexOfCoincidence;
	double measureOfRoughness;
} Analysis;

void usage();
void options_init(Options *opts);
void analysis_init(Analysis *analysis);
void frequencyPrint(Frequency *text, Frequency *lang, FILE *f);
char *argv0;

int main(int argc, char *argv[])
{
	Options opt;
	Analysis analysis;
	options_init(&opt);
	analysis_init(&analysis);

	ARGBEGIN {
		case 'l':
		{
			char *filename = EARGF(die("-l requres an argument\n"));
			FILE *f = fopen(filename, "r");
			if (!f)
				die("%s: %s\n", filename, strerror(errno));
			if (langM_init(&opt.langM))
				die("%s: %s\n", filename, strerror(errno));
			if (langM_deserialize(&opt.langM, f))
				die("%s: %s\n", filename, strerror(errno));
			fclose(f);
			break;
		}
		case 't':
		{
			char *text = EARGF(die("-t requres an argument\n"));
			opt.textLength = strlen(text);
			int bufferSize = opt.textLength + 1;
			if (!(opt.text = malloc(bufferSize * sizeof(&opt.text))))
				die("malloc: %s\n", strerror(errno));
			if (stringToAlphabet(text, opt.text) > AlphabetSubsetLangM) {
				die("text: %s\n", strerror(EINVAL));
			}
			break;
		}
		case 'h':
			usage();
			break;
		default:
			die("-%c: %s\n", ARGC(), strerror(EINVAL));
	} ARGEND;

	if (!opt.text || !opt.langM.head)
		usage();

	/* Analyze */
	frequencyLangM(&opt.langM, &analysis.lang);
	frequencyText(opt.text, &analysis.text);
	analysis.chiSquared = chiSquared(&analysis.text, &analysis.lang);
	analysis.indexOfCoincidence = indexOfCoincidence(&analysis.lang);
	analysis.measureOfRoughness = measureOfRoughness(&analysis.lang);
	printf("chi: %f\n", analysis.chiSquared);
	printf("roughness: %f\n", analysis.measureOfRoughness);
	printf("IC: %f\n", analysis.indexOfCoincidence);
	frequencyPrint(&analysis.text, &analysis.lang, stdout);
}

void options_init(Options *opts)
{
	memset(opts, 0, sizeof(*opts));
}

void analysis_init(Analysis *analysis)
{
	memset(analysis, 0, sizeof(*analysis));
}

void frequencyPrint(Frequency *text, Frequency *lang, FILE *f)
{
	fprintf(f, "Total: %d %d\n", text->n, lang->n);
	for (Alphabet a = 0; a < (int)AlphabetSubsetCipher; a++) {
		int textFreq = text->freq[a];
		double textPercent = textFreq / (double)text->n * 100;
		int langFreq = lang->freq[a];
		double langPercent = langFreq  / (double)lang->n * 100;
		fprintf(f, "%c: %d %f %d %f\n", alphabetToChar(a), textFreq, textPercent, langFreq, langPercent);
	}
}

void usage()
{
	fputs("usage: main\n", stderr);
	exit(EXIT_FAILURE);
}
