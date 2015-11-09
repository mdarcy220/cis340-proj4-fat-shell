#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lex.h"

typedef enum {false=0, true} bool;

static bool isQuote(char);
static bool isWhitespace(char);
static bool isIORedirectChar(char);
static char getEscapedChar(char);

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

	bool inQuote = false;   // To check if we're inside quoted text
	bool escapeNext = false; // To check if the character is being escaped by a backslash

	char curTok[256];
	int curTokLen = 0;

	int index;
	for (index = 0; index < strlen(inputStr) + 1; index++) {
		char ch = inputStr[index];
		
		if(escapeNext) {
			ch = getEscapedChar(ch);
			curTok[curTokLen++] = ch;
			escapeNext = false;
			continue;
		}

		if (isQuote(ch)) {
			inQuote = !inQuote; // Toggle whether we're in a quotation
		} else if (isWhitespace(ch) && curTokLen != 0 && !inQuote) {
			curTok[curTokLen++] = '\0';
			add_str_to_arr(curTok, tokenArr, nTokens, tokenArrCap);
			nTokens++;
			curTokLen = 0;
		} else if (isIORedirectChar(ch) && !inQuote) {
			if (curTokLen != 0) {
				curTok[curTokLen++] = '\0';
				add_str_to_arr(curTok, tokenArr, nTokens, tokenArrCap);
				nTokens++;
				curTokLen = 0;
			}

			curTok[0] = ch;
			curTok[1] = '\0';
			add_str_to_arr(curTok, tokenArr, nTokens, tokenArrCap);
		} else if(ch == '\\') {
			escapeNext = true;
		} else {
			curTok[curTokLen++] = ch;
		}
	}

	return nTokens;
}


// Checks if the given char is a quotation mark
static bool isQuote(char c) {
	switch(c) {
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
	switch(c) {
	case 'n':
		return '\n';
	case 't':
		return '\t';
	default:
		return c;
	}
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
