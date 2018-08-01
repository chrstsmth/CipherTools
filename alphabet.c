#include "alphabet.h"

Alphabet alphabet_add(Alphabet a, Alphabet b)
{
	return (a + b) % AlphabetSize;
}

Alphabet alphabet_subtract(Alphabet a, Alphabet b)
{
	return (a - b + AlphabetSize) % AlphabetSize;
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

void stringToAlphabet(char *c, Alphabet *a)
{
	for (; *c != '\0'; a++, c++) {
		*a = charToAlphabet(*c);
	}
	*a = AlphabetNull;
}

void alphabetToString(Alphabet *a, char *c)
{
	for (; *a != AlphabetNull; a++, c++) {
		*c = alphabetToChar(*a);
	}
	*c = '\0';
}
