#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "alphabet.h"
#include "language-model.h"

void langM_initNode(Node *node);
void langM_freeNode(Node *n);
int langM_orderNode(Node *n);
int langM_serializeNode(Node *n, FILE *f);
int langM_deserializeNode(Node *n, FILE *f);

int compareNode(const void* a, const void* b);

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
	free(node->order);
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
		if(!isAlphabetSubsetLangM(i)) {
			errno = EINVAL;
			break;
		}

		/* Get next node. Malloc if null */
		Node **next = &cursor->next[i];
		if (!*next) {
			if (!(*next = (Node*)malloc(sizeof(Node))))
				break;
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

int langM_order(LanguageModel *langM)
{
	return langM_orderNode(langM->head);
}

int langM_orderNode(Node *n)
{
	if (!n->populated)
		return 0;

	free(n->order);
	if (!(n->order = (Alphabet*)malloc(n->populated * sizeof(Alphabet))))
		return 1;

	Node **buffer[n->populated];
	Node ***cursor = buffer;
	for (int i = 0; i < (int)AlphabetSubsetLangM; i++)
	{
		Node **next = &n->next[i];
		if (*next) {
			if (langM_orderNode(*next))
				return 1;
			*cursor++ = next;
		}
	}

	qsort(buffer, n->populated, sizeof(*buffer), compareNode);
	for (int i = 0; i < n->populated; i++)
		n->order[i] = (Alphabet)(buffer[i] - n->next);

	return 0;
}

int langM_deserialize(LanguageModel *langM, FILE *f)
{
	return langM_deserializeNode(langM->head, f);
}

int langM_deserializeNode(Node *n, FILE *f)
{
	char c;
	int freq;
	Alphabet buffer[AlphabetSubsetLangM];
	Alphabet *cursor = buffer;

	if (fscanf(f, "%d", &freq) != 1) {
		errno = EINVAL;
		return 1;
	}
	n->freq+=freq;

	if ((c = getc(f)) != '(') {
		errno = EINVAL;
		return 1;
	}

	while ((c = getc(f)) != ')') {
		/* Generate index */
		Alphabet i = charToAlphabet(c);
		if(!isAlphabetSubsetLangM(i)) {
			errno = EINVAL;
			return 1;
		}

		/* Get next node. Malloc if null */
		Node **next = &n->next[i];
		if (!*next) {
			if (!(*next = (Node*)malloc(sizeof(Node))))
				return 1;
			langM_initNode(*next);
			n->populated++;
			*cursor++ = i;
		}

		if (langM_deserializeNode(*next, f))
			return 1;
	}
	if (c == EOF) {
		errno = EINVAL;
		return 1;
	}

	free(n->order);
	if (!(n->order = (Alphabet*)malloc(n->populated * sizeof(Alphabet))))
		return 1;
	memcpy(n->order, buffer, n->populated);

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

	for (int i = 0; i < n->populated; i++)
	{
		Alphabet a = n->order[i];
		Node *next = n->next[a];
		if (next) {
			if (fprintf(f ,"%c", alphabetToChar(a)) < 0)
				return 1;
			if (langM_serializeNode(next, f))
				return 1;
		}
	}
	fprintf(f ,")");
	return 0;
}

int compareNode(const void* a, const void* b)
{
	return (**(Node***)(b))->freq - (**(Node***)(a))->freq;
}
