
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
#include <stdio.h>
#include <unistd.h>
#include <string.h>


#include "internalcommands.h"


static void remove_str_from_path(char *, char *);


// Sets the Flop shell state to quit at the next opportunity
int command_quit(struct FlopShellState *flopstate, struct FlopCommand *command) {
	flopstate->hasQuit = true;
	return 0;
}


// Changes the working directory of the shell
int command_cd(struct FlopShellState *flopstate, struct FlopCommand *command) {
	if (command->argc < 2) {
		if (chdir(getenv("HOME")) != -1) {
			return 0;
		} else {
			fprintf(stderr, "Couldn't change directory.");
			return 1;
		}
	}

	if (chdir(command->argv[1]) != -1) {
		return 0;
	} else {
		fprintf(stderr, "Couldn't change directory.\n");
		return 1;
	}
}


// Adds or removes strings from the PATH variable
int command_path(struct FlopShellState *flopstate, struct FlopCommand *command) {
	if (command->argc == 1) {
		printf("%s\n", flopstate->pathVar);
		return 0;
	} else if (command->argc == 2 && strcmp(command->argv[1], "import") == 0) {
		char *newPath = getenv("PATH");
		if(newPath == NULL) {
			fprintf(stderr, "Unable to get PATH from environment");
			return 0;
		}
		
		flopstate->pathLen = strlen(newPath);
		if(flopstate->pathCap < flopstate->pathLen+1) {
			free(flopstate->pathVar);
			flopstate->pathCap = flopstate->pathLen+1;
			flopstate->pathVar = calloc(flopstate->pathCap, sizeof(char));
		}
		strcpy(flopstate->pathVar, newPath);
		return 0;
	}

	if (strcmp(command->argv[1], "+") == 0) {
		size_t newLen = flopstate->pathLen + strlen(command->argv[2]) + 1; // +1 for colon separator
		if (flopstate->pathCap < newLen + 1) {
			flopstate->pathVar = realloc(flopstate->pathVar, (newLen + 1) * sizeof(char));
		}

		if (flopstate->pathLen != 0) {
			strcat(flopstate->pathVar, ":");
		}

		strcat(flopstate->pathVar, command->argv[2]);
		flopstate->pathLen = newLen;

	} else if (strcmp(command->argv[1], "-") == 0) {
		remove_str_from_path(flopstate->pathVar, command->argv[2]);
		flopstate->pathLen = strlen(flopstate->pathVar);
		// fprintf(stderr, "This feature is not yet supported.\n");
	}

	return 0;
}


// Removes the given string from the given colon-separated list of pathnames
static void remove_str_from_path(char *path, char *str) {
	off_t pathIndex = 0;
	off_t strIndex = 0;
	size_t strLen = strlen(str);

	while (path[pathIndex] != '\0') {
		if (path[pathIndex] == ':' && strIndex == strLen) {
			// Remove substring from (pathIndex-strLen) to pathIndex
			memmove(path + (pathIndex - strIndex) * sizeof(*path),
				path + (pathIndex + 1) * sizeof(*path), 1 + strlen(path) - pathIndex);
			return;
		} else if (path[pathIndex] != str[strIndex]) {
			strIndex = -1;
		}

		pathIndex++;
		strIndex++;
	}

	if (path[pathIndex] == '\0' && strIndex == strLen) {
		// Remove substring from (pathIndex-strLen) to pathIndex
		memmove(path + (pathIndex - strIndex) * sizeof(*path), path + pathIndex * sizeof(*path),
			1 + strlen(path) - pathIndex);
	}
	if (path[pathIndex - strIndex - 1] == ':') {
		path[pathIndex - strIndex - 1] = '\0';
	}
}
