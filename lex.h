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
