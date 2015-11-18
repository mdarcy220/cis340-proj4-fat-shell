#include <stdlib.h>

#include "FlopCommand.h"

// Frees all memory associated with the given FlopCommand
void FlopCommand_destroy(struct FlopCommand* command) {
	if(command == NULL) {
		return;
	}
	
	if(command->pipeCommand != NULL) {
		FlopCommand_destroy(command->pipeCommand);
	}
	
	
	int i;
	for(i = 0; i < command->argc; i++) {
		free(command->argv[i]);
	}
	free(command->argv);
	
	free(command->inputFile);
	free(command->outputFile);
	free(command->commandName);
	free(command->execPath);
	
	free(command);
}


// Creates a new FlopCommand. The new command should only be freed using the FlopCommand_destroy method
struct FlopCommand *FlopCommand_new() {
	struct FlopCommand *command = malloc(sizeof(struct FlopCommand));
	command->commandName = NULL;
	command->argc = 0;
	command->argv = NULL;
	command->inputFile = NULL;
	command->outputFile = NULL;
	command->pipeCommand = NULL;
	command->execPath = NULL;
	
	return command;
}
