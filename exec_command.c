#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "exec_command.h"

// Include headers for built-in commands
#include "help.h"
#include "FlopCommand.h"
#include "internalcommands.h"


struct InternalCommand {
	char *commandName;
	int (*execute)(struct FlopShellState *, struct FlopCommand *);
};


static CommandType get_command_type(struct FlopCommand *);
static void get_command_exec_path(struct FlopShellState *, char *, struct FlopCommand *);

static void exec_internal(struct FlopShellState *, struct FlopCommand *);
static void exec_builtin(struct FlopShellState *, struct FlopCommand *);
static void exec_normal(struct FlopShellState *, struct FlopCommand *);


// Internal commands (from internalcommands.h)
static const struct InternalCommand INTERNAL_COMMANDS[] = {{"cd", command_cd},
							   {"path", command_path},
							   {"quit", command_quit},
							   {"q", command_quit},
							   {NULL, NULL}};

// Built-in commands that can be executed
static const char *BUILTIN_COMMAND_NAMES[] = {"fmount",     "fumount",   "help",     "showfat", "showfile",
					      "showsector", "structure", "traverse", NULL};


// Executes the given FlopCommand. The FlopShellState (struct defined in flopshell.h) gives information about
// the state of the flopshell, such as the PATH variable, mounted FlopData, etc.
void exec_command(struct FlopShellState *flopstate, struct FlopCommand *command) {
	CommandType commandType = get_command_type(command);


	switch (commandType) {
	case CommandType_Internal:
		exec_internal(flopstate, command);
		break;
	case CommandType_Builtin:
		exec_builtin(flopstate, command);
		break;
	case CommandType_Normal: {
		exec_normal(flopstate, command);
		break;
	}
	default:
		fprintf(stderr, "An error occurred.\n");
		break;
	}
}


// Execute the given internal command
static void exec_internal(struct FlopShellState *flopstate, struct FlopCommand *command) {
	int i;
	for (i = 0; INTERNAL_COMMANDS[i].commandName != NULL; i++) {
		if (strcmp(command->commandName, INTERNAL_COMMANDS[i].commandName) == 0) {
			INTERNAL_COMMANDS[i].execute(flopstate, command);
		}
	}
}


// Execute the given builtin command
static void exec_builtin(struct FlopShellState *flopstate, struct FlopCommand *command) {}


// Execute the given normal command
static void exec_normal(struct FlopShellState *flopstate, struct FlopCommand *command) {
	char *commandPath = calloc(256, sizeof(char));
	get_command_exec_path(flopstate, commandPath, command);

	// Fork and execute

	free(commandPath);
}


// Gets the type of the specified command
static CommandType get_command_type(struct FlopCommand *command) {
	int i;

	for (i = 0; BUILTIN_COMMAND_NAMES[i] != NULL; i++) {
		if (strcmp(command->commandName, BUILTIN_COMMAND_NAMES[i]) == 0) {
			return CommandType_Builtin;
		}
	}

	for (i = 0; INTERNAL_COMMANDS[i].commandName != NULL; i++) {
		if (strcmp(command->commandName, INTERNAL_COMMANDS[i].commandName) == 0) {
			return CommandType_Internal;
		}
	}

	return CommandType_Normal;
}


// Gets the path to the executable file for the given command, and stores it in the given malloc'ed buffer
static void get_command_exec_path(struct FlopShellState *flopstate, char *cmdPath, struct FlopCommand *cmd) {
	
}
