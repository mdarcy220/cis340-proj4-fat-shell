#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "parse.h"

typedef enum { false = 0, true } bool;

// Parse the given set of tokens into a command structure
struct FlopCommand *parse_flopsh(struct Token **tokArr, int nTokens) {
	if (nTokens == 0) {
		return NULL;
	}

	bool hasError = false;

	struct FlopCommand *command = FlopCommand_new();

	int tokArrIndex = 0;
	command->argc = 0;
	command->argv = malloc(sizeof(char *));

	while (tokArrIndex < nTokens) {

		if (tokArr[tokArrIndex]->tokType == tok_ioredirect) {
			// Something needs to come after the redirector, so error if there's nothing
			if (nTokens <= tokArrIndex + 1) {
				fprintf(stderr, "Syntax error. Unexpected end of input.\n");
				hasError = true;
				break;
			}

			if (tokArr[tokArrIndex]->tokStr[0] == '<') {
				free(command->inputFile); // in case it had already been set
				command->inputFile = malloc(
						(strlen(tokArr[tokArrIndex + 1]->tokStr + 1) * sizeof(char)));
				strcpy(command->inputFile, tokArr[tokArrIndex + 1]->tokStr);
			} else if (tokArr[tokArrIndex]->tokStr[0] == '>') {
				free(command->outputFile); // in case it had already been set
				command->outputFile = malloc(
						(strlen(tokArr[tokArrIndex + 1]->tokStr + 1) * sizeof(char)));
				strcpy(command->outputFile, tokArr[tokArrIndex + 1]->tokStr);
			} else if (tokArr[tokArrIndex]->tokStr[0] == '|') {
				command->pipeCommand = parse_flopsh(tokArr + tokArrIndex + 1,
								    nTokens - tokArrIndex - 1);
				break;
			} else {
				fprintf(stderr, "An unexpected error occurred while parsing input.\n");
				hasError = true;
				break;
			}

			tokArrIndex += 2;
		} else {
			command->argv = realloc(command->argv, (command->argc + 1) * sizeof(char *));
			command->argv[command->argc] =
					malloc((strlen(tokArr[tokArrIndex]->tokStr) + 1) * sizeof(char));
			strcpy(command->argv[command->argc], tokArr[tokArrIndex]->tokStr);

			tokArrIndex++;
			command->argc++;
		}
	}

	if (command->argc == 0) {
		fprintf(stderr, "Missing command.\n");
		hasError = true;
	} else {
		command->commandName = malloc((strlen(command->argv[0]) + 1) * sizeof(*command->commandName));
		strcpy(command->commandName, command->argv[0]);
	}

	if (hasError) {
		FlopCommand_destroy(command);
		return NULL;
	} else {
		return command;
	}
}
