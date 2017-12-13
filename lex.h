
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

#ifndef LEX_H_
#define LEX_H_

#include <stdlib.h>

typedef enum TokenType {
	tok_string,
	tok_ioredirect,
	tok_eof
} TokenType;


struct Token {
	TokenType tokType;
	char *tokStr;
	size_t tokStrLen;
	size_t tokStrCap;
};


struct Lexer {
	char *inputStr;
	size_t inputStrLen;
	off_t curPos;
	struct Token *curTok;
};


struct Lexer *Lexer_new(char*);
void Lexer_destroy(struct Lexer*);
void Lexer_advanceToNextToken(struct Lexer*);

// NOTE: This function is deprecated. Please don't use it in new code.
size_t lex_flopsh(char *, struct Token***, size_t *);

#endif
