#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lex.h"

typedef enum { false = 0, true } bool;

static bool isQuote(char);
static bool isWhitespace(char);
static bool isIORedirectChar(char);
static char getEscapedChar(char);

static void add_tok_to_arr(struct Token*, struct Token***, size_t, size_t *);


/*
 * Lexes a command string into an array of arguments stored in the char** given by tokenArr, and returns
 * number of arguments found. If the capacity of tokenArr is not sufficient to hold all the tokens, it will be
 * extended using realloc, and the new capacity will be stored in tokenArrCap.
 */
size_t lex_flopsh(char *inputStr, struct Token ***tokenArr, size_t *tokenArrCap) {
	if (inputStr == NULL || tokenArr == NULL || tokenArrCap == NULL || inputStr[0] == '\0') {
		return 0;
	}

	size_t nTokens = 0;

	bool inQuote = false;    // To check if we're inside quoted text
	bool escapeNext = false; // To check if the character is being escaped by a backslash
	
	struct Token *curTok = malloc(sizeof(struct Token));

	char curTokStr[256];
	int curTokStrLen = 0;

	int index;
	for (index = 0; index < strlen(inputStr) + 1; index++) {
		char ch = inputStr[index];

		if (escapeNext) {
			ch = getEscapedChar(ch);
			curTokStr[curTokStrLen++] = ch;
			escapeNext = false;
			continue;
		}

		if (isQuote(ch)) {
			inQuote = !inQuote; // Toggle whether we're in a quotation
		} else if (isWhitespace(ch) && curTokStrLen != 0 && !inQuote) {
			curTok->tokType = tok_string;
			curTokStr[curTokStrLen++] = '\0';
			curTok->tokStr = malloc((strlen(curTokStr)+1)*sizeof(char));
			strcpy(curTok->tokStr, curTokStr);
			add_tok_to_arr(curTok, tokenArr, nTokens, tokenArrCap);
			curTok = malloc(sizeof(struct Token));
			nTokens++;
			curTokStrLen = 0;
		} else if (isIORedirectChar(ch) && !inQuote) {
			if (curTokStrLen != 0) {
				curTok->tokType = tok_string;
				curTokStr[curTokStrLen++] = '\0';
				curTok->tokStr = malloc((strlen(curTokStr)+1)*sizeof(char));
			strcpy(curTok->tokStr, curTokStr);
				add_tok_to_arr(curTok, tokenArr, nTokens, tokenArrCap);
				curTok = malloc(sizeof(struct Token));
				nTokens++;
				curTokStrLen = 0;
			}

			curTokStr[0] = ch;
			curTokStr[1] = '\0';
			curTok->tokStr = malloc((strlen(curTokStr)+1)*sizeof(char));
			curTok->tokType = tok_ioredirect;
			strcpy(curTok->tokStr, curTokStr);
			add_tok_to_arr(curTok, tokenArr, nTokens, tokenArrCap);
			nTokens++;
			curTok = malloc(sizeof(struct Token));
		} else if (ch == '\\') {
			escapeNext = true;
		} else {
			curTokStr[curTokStrLen++] = ch;
		}
	}
	
	free(curTok);

	return nTokens;
}


// Checks if the given char is a quotation mark
static bool isQuote(char c) {
	switch (c) {
	case '"':
		return true;
	default:
		return false;
	}
}


// Checks if the given character is whitespace
static bool isWhitespace(char c) {
	switch (c) {
	case ' ':
	case '\0':
		return true;
	default:
		return false;
	}
}


// Checks if the given char is used to indicate IO redirection
static bool isIORedirectChar(char c) {
	switch (c) {
	case '<':
	case '>':
	case '|':
		return true;
	default:
		return false;
	}
}


// Gets the escaped version of the given char (for example, if 'n' is passed it returns '\n')
static char getEscapedChar(char c) {
	switch (c) {
	case 'n':
		return '\n';
	case 't':
		return '\t';
	default:
		return c;
	}
}


/*
// Copies the given string into the given buffer, expanding the buffer using
// realloc if necessary
static void add_tok_to_arr(struct Token *str, struct Token **arr, size_t arrLen, size_t *arrCap) {
	if (*arrCap < arrLen + 1) {
		(*arrCap)++;
		*arr = realloc(*arr, (*arrCap) * sizeof(**arr));
	}
	(*arr)[arrLen] = malloc((strlen(str) + 1) * sizeof(*str));
	strcpy((*arr)[arrLen], str);
}*/


// Puts the given Token into the given array
static void add_tok_to_arr(struct Token *tok, struct Token ***arr, size_t arrLen, size_t *arrCap) {
	if (*arrCap < arrLen + 1) {
		(*arrCap)++;
		*arr = realloc(*arr, (*arrCap) * sizeof(**arr));
	}
	(*arr)[arrLen] = tok;
}

