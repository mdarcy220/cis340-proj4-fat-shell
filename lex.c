
/*
 * Copyright (C) 2017 Mike D'Arcy
 *
 * This file is part of FlopShell, a toy shell utility for floppy disk images.
 * 
 * FlopShell is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * FlopShell is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FlopShell.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lex.h"

typedef enum { false = 0, true } bool;

static bool isQuote(char);
static bool isWhitespace(char);
static bool isIORedirectChar(char);
static char getEscapedChar(char);

static struct Token *Token_new();
static void Token_destroy(struct Token *);
static void Token_appendChar(struct Token *, char);

static void add_tok_to_arr(struct Token *, struct Token ***, size_t, size_t *);


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
			curTok->tokStr = malloc((strlen(curTokStr) + 1) * sizeof(char));
			strcpy(curTok->tokStr, curTokStr);
			add_tok_to_arr(curTok, tokenArr, nTokens, tokenArrCap);
			curTok = malloc(sizeof(struct Token));
			nTokens++;
			curTokStrLen = 0;
		} else if (isIORedirectChar(ch) && !inQuote) {
			if (curTokStrLen != 0) {
				curTok->tokType = tok_string;
				curTokStr[curTokStrLen++] = '\0';
				curTok->tokStr = malloc((strlen(curTokStr) + 1) * sizeof(char));
				strcpy(curTok->tokStr, curTokStr);
				add_tok_to_arr(curTok, tokenArr, nTokens, tokenArrCap);
				curTok = malloc(sizeof(struct Token));
				nTokens++;
				curTokStrLen = 0;
			}

			curTokStr[0] = ch;
			curTokStr[1] = '\0';
			curTok->tokStr = malloc((strlen(curTokStr) + 1) * sizeof(char));
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


// Creates a new Lexer
struct Lexer *Lexer_new(char *input) {
	if (input == NULL) {
		return NULL;
	}

	// Allocate memory and initialize it with zeros
	struct Lexer *lexer = calloc(1, sizeof(struct Lexer));

	lexer->inputStrLen = strlen(input);
	lexer->inputStr = calloc(lexer->inputStrLen + 1, sizeof(char));
	strcpy(lexer->inputStr, input);

	lexer->curPos = 0;
	lexer->curTok = Token_new();

	return lexer;
}


// Destroys the given Lexer, freeing all its memory
void Lexer_destroy(struct Lexer *lexer) {
	free(lexer->inputStr);
	Token_destroy(lexer->curTok);
	free(lexer);
}


// Advances the lexer to the next Token
void Lexer_advanceToNextToken(struct Lexer *lexer) {
	// Reset the current token's string
	lexer->curTok->tokStrLen = 0;
	lexer->curTok->tokStr[0] = '\0';

	bool inQuote = false;
	bool escapeNext = false;

	while (lexer->curPos <= lexer->inputStrLen) {
		char ch = lexer->inputStr[lexer->curPos];

		if (escapeNext) {
			ch = getEscapedChar(ch);
			Token_appendChar(lexer->curTok, ch);
			escapeNext = false;
			lexer->curPos++;
			continue;
		}

		if (isQuote(ch)) {
			inQuote = !inQuote; // Toggle whether we're in a quotation
			lexer->curPos++;
		} else if (ch == ' ' && !inQuote) {
			if (lexer->curTok->tokStrLen != 0) {
				lexer->curTok->tokType = tok_string;
				lexer->curPos++;
				return;
			}
			lexer->curPos++;
		} else if (ch == '\0') {
			if (lexer->curTok->tokStrLen != 0) {
				lexer->curTok->tokType = tok_string;
			} else {
				lexer->curTok->tokType = tok_eof;
			}
			return;
		} else if (isIORedirectChar(ch) && !inQuote) {
			if (lexer->curTok->tokStrLen != 0) {
				lexer->curTok->tokType = tok_string;
				// Don't increment lexer->curPos here. We want to grab the actual redirect
				// char next.
				return;
			}

			Token_appendChar(lexer->curTok, ch);
			lexer->curTok->tokType = tok_ioredirect;
			lexer->curPos++;
			return;
		} else if (ch == '\\') {
			escapeNext = true;
			lexer->curPos++;
		} else {
			Token_appendChar(lexer->curTok, ch);
			lexer->curPos++;
		}
	}
}


// Creates a new Token and returns it
static struct Token *Token_new() {
	struct Token *tok = calloc(1, sizeof(struct Token));
	tok->tokStrCap = 256;
	tok->tokStr = calloc(tok->tokStrCap, sizeof(char));
	tok->tokStrLen = 0;
	tok->tokType = tok_string;

	return tok;
}


// Destroys th given token
static void Token_destroy(struct Token *tok) {
	free(tok->tokStr);
	free(tok);
}


// Appends the given character to the string of the given Token
static void Token_appendChar(struct Token *tok, char c) {
	if (tok->tokStrCap < tok->tokStrLen + 2) {
		tok->tokStr = realloc(tok->tokStr, (tok->tokStrCap + 256) * sizeof(tok->tokStr[0]));
		tok->tokStrCap += 256;
	}

	tok->tokStr[tok->tokStrLen++] = c;
	tok->tokStr[tok->tokStrLen] = '\0';
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


// Gets the escaped version of the given char
static char getEscapedChar(char c) {
	switch (c) {
	// case 'n':
	//	return '\n';
	// case 't':
	//	return '\t';
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
