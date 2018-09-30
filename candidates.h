#ifndef CANDIDATES_H
#define CANDIDATES_H

#include <stdio.h>

#include "cipher.h"

typedef struct Candidate {
	Key *key;
	Alphabet *text;
	const Cipher *cipher;
	int score;
} Candidate;

typedef struct Candidates {
	Candidate **c;
	size_t n;
} Candidates;

int candidate_init(Candidate *candidate);
int candidate_copy(Candidate *candidate, Candidate *other);
void candidate_free(Candidate *candidate);
int candidate_print(Candidate *candidate, FILE *f);

int candidates_init(Candidates *candidates, size_t n);
void candidates_free(Candidates *candidates);
int candidates_copyInsert(Candidates *candidates, Candidate *candidate);
int candidates_print(Candidates *candidates, FILE *f);

#endif
