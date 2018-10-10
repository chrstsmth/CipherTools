#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "arg.h"
#include "candidates.h"
#include "cipher.h"
#include "language-model.h"
#include "util.h"

typedef enum {
	CommandEncipher,
	CommandDecipher,
	CommandDictionary,
	CommandBruteForce,
	CommandHillClimb,
} Command;

typedef struct {
	LanguageModel langM;
	const Cipher *cipher;
	Command command;
	Alphabet *textIn;
	Alphabet *textOut;
	Candidates candidates;
	int textLength;
	Key key;
	FILE *dictionary;
} Options;

static struct {
	sig_atomic_t interrupt;
} sig;

static void signal_handler(int signum);
void usage();
void die(const char *errstr, ...);
void options_init(Options *opts);
char *argv0;

int main(int argc, char *argv[])
{
	Options opt;
	options_init(&opt);
	srand(time(NULL));

	if (argc < 3)
		usage();

	/* install signal handler */
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGINT);
	if (sigaction(SIGINT, &sa, NULL) == -1)
		die("Failed to set signal handler: %s\n", strerror(errno));

	argc--; argv++;
	char *ciphername = *argv;
	if (strcmp(ciphername, "vigenere") == 0) {
		opt.cipher = &ciphers[CipherVigenere];
	} else if (strcmp(ciphername, "caesar") == 0) {
		opt.cipher = &ciphers[CipherCaesar];
	} else {
		die("%s: %s\n", ciphername, strerror(EINVAL));
	}

	argc--; argv++;
	char *command = *argv;
	if (strcmp(command, "encipher") == 0) {
		opt.command = CommandEncipher;
	} else if (strcmp(command, "decipher") == 0) {
		opt.command = CommandDecipher;
	} else if (strcmp(command, "dictionary") == 0) {
		opt.command = CommandDictionary;
	} else if (strcmp(command, "bruteForce") == 0) {
		opt.command = CommandBruteForce;
	} else if (strcmp(command, "hillClimb") == 0) {
		opt.command = CommandHillClimb;
	} else {
		die("%s: %s\n", command, strerror(EINVAL));
	}

	ARGBEGIN {
		case 'd':
		{
			char *filename = EARGF(die("-d requres an argument\n"));
			if (!(opt.dictionary = fopen(filename, "r")))
				die("%s: %s\n", filename, strerror(errno));
			break;
		}
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
			char *keyString = EARGF(die("-k requres an argument\n"));
			if (opt.cipher->k->initKey(&opt.key, keyString))
				die("key: %s\n", strerror(errno));
		}
			break;
		case 'h':
			usage();
			break;
		default:
			die("-%c: %s\n", ARGC(), strerror(EINVAL));
	} ARGEND;

	candidates_init(&opt.candidates, 30);

	switch (opt.command) {
		case CommandEncipher:
			if (!opt.textIn || !opt.key.buf)
				usage();
			if (opt.cipher->c->encipher(opt.textIn, opt.textOut, &opt.key))
				die("%s encipher: %s\n", opt.cipher->name, strerror(errno));
			break;
		case CommandDecipher:
			if (!opt.textIn || !opt.key.buf)
				usage();
			if (opt.cipher->c->decipher(opt.textIn, opt.textOut, &opt.key))
				die("%s decipher: %s\n", opt.cipher->name, strerror(errno));
			break;
		case CommandDictionary:
			if (!opt.textIn || !opt.langM.head || !opt.dictionary)
				usage();
			int line;
			if ((line = (opt.cipher->c->dictionary(opt.textIn, &opt.candidates, &opt.langM, opt.dictionary, &sig.interrupt))))
				die("%s dictionary:%d: %s\n", opt.cipher->name, line, strerror(errno));
			break;
		case CommandBruteForce:
			if (!opt.textIn || !opt.langM.head)
				usage();
			if ((opt.cipher->c->bruteForce(opt.textIn, &opt.candidates, &opt.langM, &sig.interrupt)))
				die("%s bruteForce: %s\n", opt.cipher->name, strerror(errno));
			break;
		case CommandHillClimb:
			if (!opt.textIn || !opt.langM.head)
				usage();
			if ((opt.cipher->c->hillClimb(opt.textIn, &opt.candidates, &opt.langM, &sig.interrupt)))
				die("%s hillClimb: %s\n", opt.cipher->name, strerror(errno));
			break;
	}

	switch (opt.command) {
		case CommandEncipher: /* Fall Through */
		case CommandDecipher:
		{
			char out[opt.textLength + 1];
			alphabetToString(opt.textOut, out);
			printf("%s\n", out);
			break;
		}
		case CommandBruteForce: /* Fall Through */
		case CommandDictionary: /* Fall Through */
		case CommandHillClimb:
			candidates_print(&opt.candidates, stdout);
			break;
	}

	if (opt.dictionary)
		fclose(opt.dictionary);
	opt.cipher->k->freeKey(&opt.key);
	free(opt.textIn);
	free(opt.textOut);
	langM_free(&opt.langM);
	candidates_free(&opt.candidates);
}

static void signal_handler(int signum)
{
	switch (signum) {
		case SIGINT:
			sig.interrupt = true;
	}
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
