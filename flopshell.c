#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "flopshell.h"
#include "FlopCommand.h"
#include "lex.h"
#include "parse.h"
#include "exec_command.h"
#include "flop.h"

// include shell command functions
#include "fmountutils.h"
#include "help.h"
#include "showfat.h"
#include "showfile.h"
#include "showsector.h"
#include "structure.h"
#include "traverse.h"


// Represents a shell command
struct BuiltInCommand {
	char *commandName;
	int execInChild;

	int (*execute)(struct FlopData *flopdata, int argc, char **argv);
};

typedef enum { false = 0, true } bool;


static void flopshell_run();
static bool isQuitCommand(struct FlopCommand *);
static const struct BuiltInCommand *get_shell_command(char *);


static const char *SHELL_PROMPT = "flop: ";

// Commands that can be executed
static const struct BuiltInCommand BUILTIN_COMMANDS[] = {{"fmount", 1, command_fmount},
							 {"fumount", 1, command_fumount},
							 {"help", 1, command_help},
							 {"showfat", 1, command_showfat},
							 {"showfile", 1, command_showfile},
							 {"showsector", 1, command_showsector},
							 {"structure", 1, print_fs_structure},
							 {"traverse", 1, command_traverse},
							 {0}};


// Start the flop shell
void flopshell_start() { flopshell_run(); }


// Runs the flop shell until the user enters the "quit" command
static void flopshell_run() {

	struct FlopData flopdata = {NULL, 0};
	bool userQuit = false;

	// Init user input buffer
	size_t inputBufCapacity = 256;
	char *userinput = malloc(inputBufCapacity * sizeof(*userinput));

	// Init token array
	size_t tokArrCap = 1;
	char **tokArr = malloc(tokArrCap * sizeof(char *));

	while (!userQuit) {
		printf("%s", SHELL_PROMPT);

		// Get command from user
		size_t inputlen = getline(&userinput, &inputBufCapacity, stdin);

		// Remove the newline character at the end of the buffer
		userinput[inputlen - 1] = '\0';

		// Convert the input into an array of tokens
		size_t nTokens = lex_flopsh(userinput, &tokArr, &tokArrCap);

		// Parse the array of tokens into a command
		struct FlopCommand *command = parse_flopsh(tokArr, nTokens);

		// Check if the command was a quit command
		userQuit = isQuitCommand(command);
		if (!userQuit && command != NULL) {
			// Execute the command
			exec_command(command);
		}

		// Destroy the command and free its memory
		FlopCommand_destroy(command);

		// free the tokens in the token array
		int i;
		for (i = 0; i < nTokens; i++) {
			free(tokArr[i]);
		}
	}

	// Clean up

	free(tokArr);
	free(userinput);

	if (flopdata.rawData != NULL) {
		fumount(&flopdata);
	}
}


static bool isQuitCommand(struct FlopCommand *command) {
	if(command == NULL) {
		return false;
	}
	return strcmp(command->commandName, "quit") == 0 || strcmp(command->commandName, "q") == 0;
}


// Get the appropriate shell command from its name
static const struct BuiltInCommand *get_shell_command(char *commandName) {
	int i;
	for (i = 0; BUILTIN_COMMANDS[i].commandName != 0; i++) {
		if (strcmp(commandName, BUILTIN_COMMANDS[i].commandName) == 0) {
			return &BUILTIN_COMMANDS[i];
		}
	}

	return NULL;
}
