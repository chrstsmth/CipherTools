#ifndef LANGUAGE_MODEL_H
#define LANGUAGE_MODEL_H

#include <stdio.h>

#include "alphabet.h"

typedef struct Node Node;

struct Node {
	Node *next[AlphabetSubsetLangM];
	Alphabet *order;
	int freq;
	int populated;
};

typedef struct {
	Node *head;
} LanguageModel;

int langM_init(LanguageModel *langM);
void langM_free(LanguageModel *langM);
int langM_insertWord(LanguageModel *langM, char *c);
int langM_order(LanguageModel *langM);
int langM_deserialize(LanguageModel *langM, FILE *f);
int langM_serialize(LanguageModel *langM, FILE *f);

#endif
