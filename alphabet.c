#include <stdbool.h>

#include "alphabet.h"

Alphabet alphabet_add(Alphabet a, Alphabet b)
{
	Alphabet add = a;
	if (isAlphabetSubsetCipher(a))
		add = (a + b) % AlphabetSubsetCipher;
	return add;
}

Alphabet alphabet_subtract(Alphabet a, Alphabet b)
{
	Alphabet add = a;
	if (isAlphabetSubsetCipher(a))
		add = (a - b + AlphabetSubsetCipher) % AlphabetSubsetCipher;
	return add;
}

AlphabetSubset getAlphabetSubset(Alphabet a)
{
	AlphabetSubset s;
	if (isAlphabetSubsetCipher(a)) {
		s = AlphabetSubsetCipher;
	} else if (isAlphabetSubsetLangM(a)) {
		s = AlphabetSubsetLangM;
	} else {
		s = AlphabetSuperset;
	}
	return s;
}

bool isAlphabetSubsetCipher(Alphabet a)
{
	return (a < (Alphabet)AlphabetSubsetCipher);
}

bool isAlphabetSubsetLangM(Alphabet a)
{
	return (a < (Alphabet)AlphabetSubsetLangM);
}

bool isAlphabetSubsetAll(Alphabet a)
{
	return (a < (Alphabet)AlphabetSuperset);
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
			case AlphabetUnknown: /* Fall through */
			default: /* Fall through */
				c = '?';
				break;
		}
	}
	return c;
}

AlphabetSubset stringToAlphabet(char *c, Alphabet *a)
{
	Alphabet max = 0;
	for (; *c != '\0'; a++, c++) {
		*a = charToAlphabet(*c);
		max = (*a > max) ? *a : max;
	}
	*a = AlphabetNull;
	return getAlphabetSubset(max);
}

void alphabetToString(Alphabet *a, char *c)
{
	for (; *a != AlphabetNull; a++, c++) {
		*c = alphabetToChar(*a);
	}
	*c = '\0';
}

int alphabetStrlen(Alphabet *a)
{
	int i;
	for (i = 0; a[i] != AlphabetNull; i++)
		continue;
	return i;
}
