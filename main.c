#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "cipher.h"
#include "language-model.h"

typedef enum {
	CommandEncipher,
	CommandDecipher,
	CommandCrack,
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
void die(const char *errstr, ...);
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
		die("%s: %s\n", ciphername, strerror(EINVAL));
	}

	argc--; argv++;
	char *command = *argv;
	if (strcmp(command, "encipher") == 0) {
		opt.command = CommandEncipher;
	} else if (strcmp(command, "decipher") == 0) {
		opt.command = CommandDecipher;
	} else if (strcmp(command, "crack") == 0) {
		opt.command = CommandCrack;
	} else {
		die("%s: %s\n", command, strerror(EINVAL));
	}

	ARGBEGIN {
		case 'l':
		{
			char *filename = EARGF(die("-l requres an argument\n"));
			FILE *in = fopen(filename, "r");
			if (!in)
				die("%s: %s\n", filename, strerror(errno));
			if (langM_init(&opt.langM))
				die("%s: %s\n", filename, strerror(errno));
			if (langM_deserialize(&opt.langM, in))
				die("%s: %s\n", filename, strerror(errno));
			break;
		}
		case 't':
		{
			char *text = EARGF(die("-t requres an argument\n"));
			opt.textLength = strlen(text);
			int bufferSize = opt.textLength + 1;
			if (!(opt.textIn = malloc(bufferSize * sizeof(&opt.textIn))))
				die("malloc: %s\n", strerror(errno));
			if (!(opt.textOut = malloc(bufferSize * sizeof(&opt.textOut))))
				die("malloc: %s\n", strerror(errno));
			if (stringToAlphabet(text, opt.textIn) > AlphabetSubsetLangM) {
				die("text: %s\n", strerror(EINVAL));
			}
			break;
		}
		case 'k':
		{
			char *key = EARGF(die("-k requres an argument\n"));
			if (!(opt.key = malloc(opt.cipher.keySize(key))))
				die("malloc: %s\n", strerror(errno));
			if (opt.cipher.parseKey(key, opt.key))
				die("key: %s\n", strerror(errno));
		}
			break;
		case 'h':
			usage();
			break;
		default:
			die("-%c: %s\n", ARGC(), strerror(EINVAL));
	} ARGEND;

	switch (opt.command) {
		case CommandEncipher:
			if (!opt.textIn || !opt.key)
				usage();
			if (opt.cipher.encipher(opt.textIn, opt.textOut, opt.key))
				die("encipher: %s\n", strerror(errno));
			break;
		case CommandDecipher:
			if (!opt.textIn || !opt.key)
				usage();
			if (opt.cipher.decipher(opt.textIn, opt.textOut, opt.key))
				die("decipher: %s\n", strerror(errno));
			break;
		case CommandCrack:
			if (!opt.textIn || !opt.langM.head)
				usage();
			if (opt.cipher.crack(opt.textIn, opt.textOut, &opt.langM))
				die("crack: %s\n", strerror(errno));
			break;
	}

	char out[opt.textLength + 1];
	alphabetToString(opt.textOut, out);
	printf("%s\n", out);

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

void die(const char *errstr, ...)
{
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

void options_init(Options *opts)
{
	memset(opts, 0, sizeof(*opts));
}
