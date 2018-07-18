#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "language-model.h"

void langM_initNode(Node *node);
void langM_freeNode(Node *n);
int langM_serializeNode(Node *n, FILE *f);
int langM_deserializeNode(Node *n, FILE *f);

int langM_init(LanguageModel *langM)
{
	memset(langM, 0, sizeof(*langM));
	if (!(langM->head = (Node*)malloc(sizeof(Node))))
		return 1;
	langM_initNode(langM->head);
	return 0;
}

void langM_free(LanguageModel *langM)
{
	langM_freeNode(langM->head);
}

void langM_initNode(Node *node)
{
	memset(node, 0, sizeof(Node));
}

void langM_freeNode(Node *node)
{
	if (!node)
		return;

	for (int i = 0; i < AlphabetSize; i++)
	{
		Node *next = node->next[i];
		if (next)
			langM_freeNode(next);
	}
	free(node);
	node = 0;
}

int langM_insertWord(LanguageModel *langM, char *c)
{
	Node *cursor = langM->head;
	char *c0 = c;

	for (; *c != '\0'; c++) {
		/* Generate index */
		Alphabet i = charToAlphabet(*c);
		if (i >= AlphabetSize)
			break;

		/* Get next node. Malloc if null */
		Node **next = &cursor->next[i];
		if (!*next) {
			if (!(*next = (Node*)malloc(sizeof(Node)))) {
				errno = ENOMEM;
				break;
			}
			langM_initNode(*next);
		}

		cursor = *next;
		cursor->freq++;
	}

	int depth = c - c0;
	if (depth)
		langM->head->freq++;
	return depth;
}

int langM_deserialize(LanguageModel *langM, FILE *f)
{
	return langM_deserializeNode(langM->head, f);
}

int langM_deserializeNode(Node *n, FILE *f)
{
	char c;
	int freq;

	if (fscanf(f, "%d", &freq) != 1)
		return 1;
	n->freq+=freq;

	if ((c = getc(f)) != '(')
		return 1;

	while ((c = getc(f)) != ')') {
		/* Generate index */
		Alphabet i = charToAlphabet(c);
		if (i >= AlphabetSize)
			return 1;

		/* Get next node. Malloc if null */
		Node **next = &n->next[i];
		if (!*next) {
			if (!(*next = (Node*)malloc(sizeof(Node)))) {
				errno = ENOMEM;
				return 1;
			}
			langM_initNode(*next);
		}

		if (langM_deserializeNode(*next, f))
			return 1;
	}
	return 0;
}
int langM_serialize(LanguageModel *langM, FILE *f)
{
	return langM_serializeNode(langM->head, f);
}

/* n must not be null */
int langM_serializeNode(Node *n, FILE *f)
{
	if (fprintf(f ,"%d(", n->freq) < 0)
		return 1;

	for (int i = 0; i < AlphabetSize; i++)
	{
		Node *next = n->next[i];
		if (next) {
			if (fprintf(f ,"%c", alphabetToChar((Alphabet)i)) < 0)
				return 1;
			langM_serializeNode(next, f);
		}
	}
	fprintf(f ,")");
	return 0;
}

Alphabet charToAlphabet(char c)
{
	Alphabet a;
	if (c >= 'a' && c <= 'z') {
		a = (Alphabet)(c - 'a');
	} else if (c >= 'A' && c <= 'Z') {
		a = (Alphabet)(c - 'A');
	} else {
		switch (c)
		{
			case ' ':
				a = AlphabetSpace;
				break;
			case '.':
				a = AlphabetPeriod;
				break;
			case '\0':
				a = AlphabetNull;
				break;
			default:
				a = '?';
				break;
		}
	}
	return a;
}

char alphabetToChar(Alphabet a)
{
	char c;

	if (a >= AlphabetA && a <= AlphabetZ) {
		c = (char)a + 'A';
	} else {
		switch (a)
		{
			case AlphabetSpace:
				c = ' ';
				break; case AlphabetPeriod: c = '.';
				break;
			case AlphabetNull:
				c = '\0';
				break;
			case AlphabetSize:
			case AlphabetUnknown: /* Fall through */
			default: /* Fall through */
				c = '?';
				break;
		}
	}
	return c;
}

int langM_insertFile(LanguageModel *langM, int depth, FILE * in)
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
