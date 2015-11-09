#include <stdlib.h>
#include <string.h>

#include "lex.h"

static void add_str_to_arr(char *, char ***, size_t, size_t *);

/*
 * Lexes a command string into an array of arguments stored in the char** given by tokenArr, and returns
 * number of arguments found. If the capacity of tokenArr is not sufficient to hold all the tokens, it will be
 * extended using realloc, and the new capacity will be stored in tokenArrCap.
 */
size_t lex_flopsh(char *inputStr, char ***tokenArr, size_t *tokenArrCap) {
	if (inputStr == NULL || tokenArr == NULL || tokenArrCap == NULL || inputStr[0] == '\0') {
		return 0;
	}

	size_t nTokens = 0;

	char delim = ' ';

	char curTok[256];
	int curTokLen = 0;

	int index;
	for (index = 0; index < strlen(inputStr) + 1; index++) {
		if ((inputStr[index] == delim || inputStr[index] == '\0') && curTokLen != 0) {
			curTok[curTokLen++] = '\0';
			add_str_to_arr(curTok, tokenArr, nTokens, tokenArrCap);
			nTokens++;
			curTokLen = 0;
		} else if (inputStr[index] != delim) {
			curTok[curTokLen++] = inputStr[index];
		}
	}

	return nTokens;
}

// Copies the given string into the given buffer, expanding the buffer using
// realloc if necessary
static void add_str_to_arr(char *str, char ***arr, size_t arrLen, size_t *arrCap) {
	if (*arrCap < arrLen + 1) {
		(*arrCap)++;
		*arr = realloc(*arr, (*arrCap) * sizeof(**arr));
	}
	(*arr)[arrLen] = malloc((strlen(str) + 1) * sizeof(*str));
	strcpy((*arr)[arrLen], str);
}
