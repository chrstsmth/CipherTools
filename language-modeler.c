#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "alphabet.h"
#include "language-model.h"

#include "arg.h"

#define OptionsDefaultDepth 5;

typedef struct {
	int depth;
} Options;

int insertFile(LanguageModel *langM, int depth, FILE * in);
void options_init(Options *opts);
bool options_verify(Options *opts);
void usage();
char *argv0;

int main(int argc, char *argv[])
{
	Options opt;
	options_init(&opt);

	if (argc <= 1)
		usage();

	ARGBEGIN {
		case 'd':
			opt.depth = strtol(EARGF(usage()), NULL, 10);
			break;
		case 'h':
		default: /* Fall through */
			usage();
	} ARGEND;

	if (!options_verify(&opt) || !argc)
		usage();

	LanguageModel langM;
	langM_init(&langM);

	for (int i = 0; i < argc; i++) {
		char *filename = argv[i];
		FILE *in = fopen(filename, "r");
		if (!in) {
			printf("can't read %s\n", filename);
			continue;
		}

		insertFile(&langM, opt.depth, in);
		fclose(in);
	}

	langM_order(&langM);
	langM_serialize(&langM, stdout);
}

void usage()
{
	fputs("usage: language-modeler [-d <depth>] file...\n", stderr);
	exit(EXIT_FAILURE);
}

void options_init(Options *opts)
{
	memset(opts, 0, sizeof(*opts));
	opts->depth = OptionsDefaultDepth;
}

bool options_verify(Options *opts)
{
	bool valid = true;
	if (opts->depth <= 0)
		valid = false;
	return valid;
}

int insertFile(LanguageModel *langM, int depth, FILE * in)
{
	char buff[depth];
	char word[depth + 1];
	int i = 0;
	char c;

	/* intialize circular buffer */
	for (i = 0; i < depth; i++) {
		c = fgetc(in);
		if (c == EOF)
			break;
		buff[i] = c;
	}

	/* insert first word if we have one*/
	if (i > 0) {
		memcpy(word, buff, i);
		word[i] = '\0';
		if (langM_insertWord(langM, word) != depth)
			return 1;
	}

	if (c == EOF)
		return 0;

	/* insert remaining words */
	i = 0;
	while ((c = fgetc(in)) != EOF) {
		buff[i] = c;
		for (int j = 0; j < depth; j++)
			word[j] = buff[(i + j + 1) % depth];
		if (langM_insertWord(langM, word) != depth)
			return 1;
		i = (i + 1) % depth;
	}

	return 0;
}
