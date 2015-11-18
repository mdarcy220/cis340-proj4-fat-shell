#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

#include "exec_command.h"
#include "FlopCommand.h"


static CommandType get_command_type(struct FlopShellState *, struct FlopCommand *);
static void get_command_exec_path(struct FlopShellState *, struct FlopCommand *);
static int find_exec_in_dir(char *, char *);
static int is_valid_file(char *);

static void exec_internal(struct FlopShellState *, struct FlopCommand *);
static void exec_normal(struct FlopShellState *, struct FlopCommand *);


// Built-in commands that can be executed
static const char *BUILTIN_COMMAND_NAMES[] = {"fmount",     "fumount",   "help",     "showfat", "showfile",
					      "showsector", "structure", "traverse", NULL};


// Executes the given FlopCommand. The FlopShellState (struct defined in flopshell.h) gives information about
// the state of the flopshell, such as the PATH variable, mounted FlopData, etc.
void exec_command(struct FlopShellState *flopstate, struct FlopCommand *command) {
	command->type = get_command_type(flopstate, command);

	if (command->type == CommandType_Internal) {
		exec_internal(flopstate, command);
	} else if (command->type == CommandType_Builtin || command->type == CommandType_Normal) {
		exec_normal(flopstate, command);
	} else {
		fprintf(stderr, "An error occurred.\n");
	}
}


// Execute the given internal command
static void exec_internal(struct FlopShellState *flopstate, struct FlopCommand *command) {
	int i;
	for (i = 0; flopstate->internalCmds[i].commandName != NULL; i++) {
		if (strcmp(command->argv[0], flopstate->internalCmds[i].commandName) == 0) {
			flopstate->internalCmds[i].execute(flopstate, command);
		}
	}
}


// Execute the given normal command
static void exec_normal(struct FlopShellState *flopstate, struct FlopCommand *command) {
	// Get path to executable into command->execPath
	get_command_exec_path(flopstate, command);

	if (command->execPath == NULL) {
		return;
	}
	
	// The execPath in command may not actually be an executable file. Only it's existence is guaranteed.
	// Therefore, execve may fail.

	// TODO: Fork and execute
	
}


// Gets the type of the specified command
static CommandType get_command_type(struct FlopShellState *flopstate, struct FlopCommand *command) {
	int i;

	for (i = 0; BUILTIN_COMMAND_NAMES[i] != NULL; i++) {
		if (strcmp(command->argv[0], BUILTIN_COMMAND_NAMES[i]) == 0) {
			return CommandType_Builtin;
		}
	}

	for (i = 0; flopstate->internalCmds[i].commandName != NULL; i++) {
		if (strcmp(command->argv[0], flopstate->internalCmds[i].commandName) == 0) {
			return CommandType_Internal;
		}
	}

	return CommandType_Normal;
}


// Gets the path to the executable file for the given command, and stores it in the execPath of the command
static void get_command_exec_path(struct FlopShellState *flopstate, struct FlopCommand *cmd) {

	if (cmd->execPath == NULL) {
		cmd->execPath = calloc(flopstate->pathLen + strlen(cmd->argv[0]) + 2, sizeof(char));
	} else {
		cmd->execPath = realloc(cmd->execPath,
					(flopstate->pathLen + strlen(cmd->argv[0]) + 2) * sizeof(char));
	}

	// If there is a '/' in the command name, it can't be a filename, so assume the user gave full path
	if (strchr(cmd->argv[0], '/') != NULL) {
		if (is_valid_file(cmd->argv[0])) {
			strcpy(cmd->execPath, cmd->argv[0]);
		} else {
			fprintf(stderr, "%s: command not found\n", cmd->argv[0]);
			free(cmd->execPath);
			cmd->execPath = NULL;
		}
		return;
	}


	// If command is built-in, search in current dir first
	if (cmd->type == CommandType_Builtin && find_exec_in_dir("./", cmd->argv[0])) {
		cmd->execPath[0] = '.';
		cmd->execPath[1] = '/';
		cmd->execPath[2] = '\0';
		strcat(cmd->execPath, cmd->argv[0]);
		return;
	}

	
	// Search through the path variable until command is found
	off_t pathVarPos = 0;
	off_t execPathPos = 0;

	while (pathVarPos <= flopstate->pathLen) {
		if (flopstate->pathVar[pathVarPos] == ':' || flopstate->pathVar[pathVarPos] == '\0') {
			cmd->execPath[execPathPos] = '\0';
			if (find_exec_in_dir(cmd->execPath, cmd->argv[0])) {
				cmd->execPath[execPathPos] = '/';
				cmd->execPath[execPathPos + 1] = '\0';
				strcat(cmd->execPath, cmd->argv[0]);
				return;
			}
			execPathPos = 0;
			pathVarPos++;
			continue;
		}
		cmd->execPath[execPathPos] = flopstate->pathVar[pathVarPos];
		pathVarPos++;
		execPathPos++;
	}

	// If command was not found, print the appropriate message and set the execPath to NULL
	fprintf(stderr, "%s: command not found\n", cmd->argv[0]);
	free(cmd->execPath);
	cmd->execPath = NULL;
}


// Returns 1 if the given command can be found in the given directory, 0 otherwise
static int find_exec_in_dir(char *dirPath, char *cmdName) {
	// Check if path can be accessed
	struct stat pathStats;
	if (stat(dirPath, &pathStats) == -1) {
		return 0;
	}

	// Check if path is directory
	if (!S_ISDIR(pathStats.st_mode)) {
		return 0;
	}

	// Try to open directory
	DIR *dirp = opendir(dirPath);
	if (dirp == NULL) {
		return 0;
	}

	struct dirent *dirinfo;
	while ((dirinfo = readdir(dirp)) != NULL) {
		if (strcmp(dirinfo->d_name, cmdName) == 0) {
			return 1;
		}
	}

	closedir(dirp);

	return 0;
}


// Checks if the given path is a valid executable file, and returns 1 if so, 0 if not
static int is_valid_file(char *path) {
	struct stat pathStats;
	if (stat(path, &pathStats) == -1) {
		return 0;
	}

	if (S_ISDIR(pathStats.st_mode)) {
		return 0;
	}

	return 1;
}
