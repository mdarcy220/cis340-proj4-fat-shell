#ifndef LEX_H_
#define LEX_H_

#include <stdlib.h>

typedef enum TokenType {
	tok_string,
	tok_ioredirect
} TokenType;


struct Token {
	TokenType tokType;
	char *tokStr;
};

size_t lex_flopsh(char *, struct Token***, size_t *);

#endif
