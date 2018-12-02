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

void usage();
void options_init(Options *opts);
char *argv0;

int main(int argc, char *argv[])
{
	Options opt;
	Analysis analysis;
	options_init(&opt);

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
	analysis_init(&analysis, opt.text, &opt.langM);
	analysis_print(&analysis, stdout);
}

void options_init(Options *opts)
{
	memset(opts, 0, sizeof(*opts));
}

void usage()
{
	fputs("usage: main\n", stderr);
	exit(EXIT_FAILURE);
}
