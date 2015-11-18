#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "parse.h"

typedef enum { false = 0, true } bool;


static int process_ioredir_tok(struct Lexer *, struct FlopCommand *);


// Parse the given set of tokens into a command structure
struct FlopCommand *parse_flopsh(char *input) {
	if (input == NULL) {
		return NULL;
	}

	bool hasError = false;

	struct FlopCommand *cmd = FlopCommand_new();
	struct Lexer *lexer;

	cmd->argc = 0;
	cmd->argv = calloc(1, sizeof(char *));

	lexer = Lexer_new(input);
	Lexer_advanceToNextToken(lexer);
	while (lexer->curTok->tokType != tok_eof) {
		if (lexer->curTok->tokType == tok_ioredirect) {
			if(process_ioredir_tok(lexer, cmd) != 0) {
				hasError = true;
				break;
			}
		} else {
			cmd->argv = realloc(cmd->argv, (cmd->argc + 1) * sizeof(char *));
			cmd->argv[cmd->argc] = calloc(strlen(lexer->curTok->tokStr) + 1, sizeof(char));
			strcpy(cmd->argv[cmd->argc], lexer->curTok->tokStr);

			cmd->argc++;
		}

		Lexer_advanceToNextToken(lexer);
	}

	if (cmd->argc != 0)  {
		cmd->commandName = calloc(strlen(cmd->argv[0]) + 1, sizeof(*cmd->commandName));
		strcpy(cmd->commandName, cmd->argv[0]);
	}

	Lexer_destroy(lexer);

	if (hasError || cmd->argc == 0) {
		FlopCommand_destroy(cmd);
		return NULL;
	} else {
		return cmd;
	}
}


// Process IO redirection symbols and put them into the cmmand appropriately. Returns 0 on success.
static int process_ioredir_tok(struct Lexer *lexer, struct FlopCommand *cmd) {
	// Keep a copy of the actual redirector character to test against
	char redirChar = lexer->curTok->tokStr[0];
	off_t redirPos = lexer->curPos;

	// Something needs to come after the redirector, so error if there's nothing
	Lexer_advanceToNextToken(lexer);
	if (lexer->curTok->tokType == tok_eof) {
		fprintf(stderr, "Syntax error. Unexpected end of input.\n");
		return 1;
	}

	if (redirChar == '<') {
		free(cmd->inputFile); // in case it had already been set
		cmd->inputFile = calloc(lexer->curTok->tokStrLen + 1, sizeof(char));
		strcpy(cmd->inputFile, lexer->curTok->tokStr);
	} else if (redirChar == '>') {
		free(cmd->outputFile); // in case it had already been set
		cmd->outputFile = calloc(lexer->curTok->tokStrLen + 1, sizeof(char));
		strcpy(cmd->outputFile, lexer->curTok->tokStr);
	} else if (redirChar == '|') {
		// Parse the subcommand, starting at the location of the pipe character
		cmd->pipeCommand = parse_flopsh(lexer->inputStr + redirPos);
	} else {
		fprintf(stderr, "An unexpected error occurred while parsing input.\n");
		return 1;
	}
	
	return 0;
}
