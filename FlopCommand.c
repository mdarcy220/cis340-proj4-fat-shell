
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
