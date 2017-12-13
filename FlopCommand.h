
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

#ifndef FLOP_COMMAND_H_
#define FLOP_COMMAND_H_


// Types of commands we can have
typedef enum CommandType {
	CommandType_Unknown,
	CommandType_Builtin,
	CommandType_Internal,
	CommandType_Normal
	
} CommandType;


// Represents a shell command
struct FlopCommand {
	char *commandName;
	char *execPath;
	int argc;
	char **argv;
	char *inputFile;
	char *outputFile;
	struct FlopCommand *pipeCommand;
	CommandType type;
};

void FlopCommand_destroy(struct FlopCommand*);
struct FlopCommand *FlopCommand_new();

#endif
