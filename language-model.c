#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "alphabet.h"
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

	for (int i = 0; i < (int)AlphabetSubsetLangM; i++)
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
		if(!isAlphabetSubsetLangM(i))
			break;

		/* Get next node. Malloc if null */
		Node **next = &cursor->next[i];
		if (!*next) {
			if (!(*next = (Node*)malloc(sizeof(Node)))) {
				errno = ENOMEM;
				break;
			}
			langM_initNode(*next);
			cursor->populated++;
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
		if(!isAlphabetSubsetLangM(i))
			return 1;

		/* Get next node. Malloc if null */
		Node **next = &n->next[i];
		if (!*next) {
			if (!(*next = (Node*)malloc(sizeof(Node)))) {
				errno = ENOMEM;
				return 1;
			}
			langM_initNode(*next);
			n->populated++;
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

int langM_serializeNode(Node *n, FILE *f)
{
	if (fprintf(f ,"%d(", n->freq) < 0)
		return 1;

	for (int i = 0; i < (int)AlphabetSubsetLangM; i++)
	{
		Node *next = n->next[i];
		if (next) {
			if (fprintf(f ,"%c", alphabetToChar((Alphabet)i)) < 0)
				return 1;
			if (langM_serializeNode(next, f))
				return 1;
		}
	}
	fprintf(f ,")");
	return 0;
}
