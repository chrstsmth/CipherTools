#include <stdlib.h>
#include <string.h>

#include "alphabet.h"
#include "candidates.h"
#include "cipher.h"

int candidates_init(Candidates *candidates, size_t n)
{
	memset(candidates, 0, sizeof(*candidates));
	candidates->n = n;
	if (!(candidates->c = (Candidate**)malloc(sizeof(*candidates->c) * candidates->n)))
		return 1;
	memset(candidates->c, 0, sizeof(*candidates->c) * candidates->n);
	return 0;
}

void candidates_free(Candidates *candidates)
{
	for (int i = 0; i < candidates->n; i++) {
		candidate_free(candidates->c[i]);
		free(candidates->c[i]);
	}
	free(candidates->c);
}

int candidates_print(Candidates *candidates, FILE *f)
{
	for (int i = 0; i < candidates->n; i++) {
		if (candidate_print(candidates->c[i], f))
			return 1;
		if (fprintf(f, "\n") < 0)
			return 1;
	} return 0;
}

int candidates_copyInsert(Candidates *candidates, Candidate *candidate)
{
	Candidate *last = candidates->c[candidates->n - 1];

	/* Exit early if no insertion will be made */
	if (last && last->score > candidate->score)
		return 0;

	/* Determine where the candidate should be inserted */
	int i;
	for (i = 0; i < candidates->n; i++) {
		Candidate *contender = candidates->c[i];
		if (!contender || candidate->score > contender->score)
			break;
		/* Don't insert the same candidate twice */
		if (candidate_equal(candidate, contender))
			return 0;
	}

	/* Shift elements to make space for new candidate */
	for (int j = candidates->n - 2; j >= i; j--)
		candidates->c[j+1] = candidates->c[j];
	free(last);

	/* Insert new element */
	Candidate *c;
	if (!(c = (Candidate*)malloc(sizeof(*c))))
		return 1;
	candidate_copy(c, candidate);
	candidates->c[i] = c;
	return 0;
}

int candidate_init(Candidate *candidate)
{
	memset(candidate, 0, sizeof(*candidate));
	return 0;
}

void candidate_free(Candidate *candidate)
{
	if (!candidate)
		return;
	freeKey(candidate->key);
	free(candidate->key);
	free(candidate->text);
}

int candidate_print(Candidate *candidate, FILE *f)
{
	if (!candidate)
		return 0;
	char strText[alphabetStrlen(candidate->text) + 1];
	alphabetToString(candidate->text, strText);
	if (fprintf(f, "(%s) ", strText) < 0)
		return 1;
	if (candidate->cipher->k->serializeKey(candidate->key, stdout))
		return 1;

	return 0;
}

int candidate_copy(Candidate *candidate, Candidate *other)
{
	candidate_init(candidate);

	if (!(candidate->key = (Key*)malloc(sizeof(candidate->key))))
		return 1;
	copyKey(candidate->key, other->key);

	int textLen = alphabetStrlen(other->text) + 1;
	if (!(candidate->text = (Alphabet*)malloc(sizeof(other->text) * textLen)))
		return 1;
	memcpy(candidate->text, other->text, sizeof(other->text) * textLen);

	candidate->cipher = other->cipher;
	candidate->score = other->score;
	return 0;
}

int candidate_equal(Candidate *candidate, Candidate *other)
{
	if (candidate->score != other->score)
		return 0;
	if (candidate->cipher != other->cipher)
		return 0;
	if (!candidate->cipher->k->equalKey(candidate->key, other->key))
		return 0;
	return 1;
}
