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
	} else if (command->argc == 2) {
		fprintf(stderr, "Invalid number of arguments.\n");
		return 1;
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
