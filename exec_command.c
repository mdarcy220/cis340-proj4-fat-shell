#include <string.h>

#include "exec_command.h"

// Include headers for built-in commands
#include "fmountutils.h"
#include "help.h"
#include "showfat.h"
#include "showfile.h"
#include "showsector.h"
#include "structure.h"
#include "traverse.h"


// Types of commands we can have
typedef enum CommandType { CommandType_Builtin, CommandType_Internal, CommandType_Normal } CommandType;


// Represents a shell command
struct BuiltInCommand {
	char *commandName;
	int execInChild;

	int (*execute)(struct FlopData *flopdata, int argc, char **argv);
};


// Built-in commands that can be executed
static const struct BuiltInCommand BUILTIN_COMMANDS[] = {{"fmount", 1, command_fmount},
							 {"fumount", 1, command_fumount},
							 {"help", 1, command_help},
							 {"showfat", 1, command_showfat},
							 {"showfile", 1, command_showfile},
							 {"showsector", 1, command_showsector},
							 {"structure", 1, print_fs_structure},
							 {"traverse", 1, command_traverse},
							 {0}};


static CommandType get_command_type(struct FlopCommand *);
static const struct BuiltInCommand *get_builtin_command(char *);


// Executes the given FlopCommand. The FlopShellState (struct defined in flopshell.h) gives information about
// the state of the flopshell, such as the PATH variable, mounted FlopData, etc.
void exec_command(struct FlopShellState *flopstate, struct FlopCommand *command) {
	CommandType commandType = get_command_type(command);
}


// Gets the type of the specified command
static CommandType get_command_type(struct FlopCommand *command) {
	int i;
	for (i = 0; BUILTIN_COMMANDS[i].commandName != 0; i++) {
		if (strcmp(command->commandName, BUILTIN_COMMANDS[i].commandName) == 0) {
			return CommandType_Builtin;
		}
	}

	// Internal commands are special, so check them individually
	if (strcmp(command->commandName, "path") == 0) {
		return CommandType_Internal;
	}
	if (strcmp(command->commandName, "cd") == 0) {
		return CommandType_Internal;
	}
	if (strcmp(command->commandName, "quit") == 0) {
		return CommandType_Internal;
	}
	if (strcmp(command->commandName, "q") == 0) {
		return CommandType_Internal;
	}

	return CommandType_Normal;
}


// Get the appropriate built-in command from its name
static const struct BuiltInCommand *get_builtin_command(char *commandName) {
	int i;
	for (i = 0; BUILTIN_COMMANDS[i].commandName != 0; i++) {
		if (strcmp(commandName, BUILTIN_COMMANDS[i].commandName) == 0) {
			return &BUILTIN_COMMANDS[i];
		}
	}

	return NULL;
}
