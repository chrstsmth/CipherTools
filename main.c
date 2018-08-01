#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "cipher.h"
#include "language-model.h"

typedef enum {
	CommandEncipher,
	CommandDecipher,
} Command;

typedef struct {
	LanguageModel langM;
	Cipher cipher;
	Command command;
	Alphabet *textIn;
	Alphabet *textOut;
	int textLength;
	void *key;
} Options;

void usage();
void options_init(Options *opts);
char *argv0;

int main(int argc, char *argv[])
{
	Options opt;
	options_init(&opt);

	if (argc < 3)
		usage();

	argc--; argv++;
	char *ciphername = *argv;
	if (strcmp(ciphername, "vigenere") == 0) {
		opt.cipher = ciphers[CipherVigenere];
	} else if (strcmp(ciphername, "caesar") == 0) {
		opt.cipher = ciphers[CipherCaesar];
	} else {
		usage();
	}

	argc--; argv++;
	char *command = *argv;
	if (strcmp(command, "encipher") == 0) {
		opt.command = CommandEncipher;
	} else if (strcmp(command, "decipher") == 0) {
		opt.command = CommandDecipher;
	} else {
		usage();
	}

	ARGBEGIN {
		case 'l':
		{
			FILE *in = fopen(EARGF(usage()), "r");
			if (!in)
				usage();
			if (langM_init(&opt.langM))
				usage();
			if (langM_deserialize(&opt.langM, in))
				usage();
			break;
		}
		case 't':
		{
			char *text = EARGF(usage());
			opt.textLength = strlen(text);
			int bufferSize = opt.textLength + 1;
			if (!(opt.textIn = malloc(bufferSize * sizeof(&opt.textIn))))
				exit(1);
			if (!(opt.textOut = malloc(bufferSize * sizeof(&opt.textOut))))
				exit(1);
			stringToAlphabet(text, opt.textIn);
			break;
		}
		case 'k':
		{
			char *key = EARGF(usage());
			if (!(opt.key = malloc(opt.cipher.keySize(key))))
				exit(1);
			if (opt.cipher.parseKey(key, opt.key))
				usage();
		}
			break;
		case 'h': /* Fall through */
		default:
			usage();
	} ARGEND;

	switch (opt.command) {
		case CommandEncipher:
			if (!opt.textIn || !opt.key)
				usage();
			opt.cipher.encipher(opt.textIn, opt.textOut, opt.key);
			break;
		case CommandDecipher:
			if (!opt.textIn || !opt.key)
				usage();
			opt.cipher.decipher(opt.textIn, opt.textOut, opt.key);
			break;
	}

	char out[opt.textLength + 1];
	alphabetToString(opt.textOut, out);
	printf("%s", out);

	free(opt.key);
	free(opt.textIn);
	free(opt.textOut);
	langM_free(&opt.langM);
}

void usage()
{
	fputs("usage: main\n", stderr);
	exit(EXIT_FAILURE);
}

void options_init(Options *opts)
{
	memset(opts, 0, sizeof(*opts));
}
