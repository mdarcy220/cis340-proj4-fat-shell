#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "flopshell.h"
#include "FlopCommand.h"
#include "lex.h"
#include "parse.h"
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


static void flopshell_run();
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

	for (;;) {
		// Get command from user
		size_t inputBufCapacity = 256;
		char *userinput = malloc(inputBufCapacity * sizeof(*userinput));
		printf("%s", SHELL_PROMPT);
		size_t inputlen = getline(&userinput, &inputBufCapacity, stdin);

		// Remove the newline character at the end of the buffer
		userinput[inputlen - 1] = '\0';

		// Parse the input into a list of command arguments
		size_t tokArrCap = 1;
		char** tokArr = malloc(tokArrCap * sizeof(char *));
		size_t nTokens = lex_flopsh(userinput, &tokArr, &tokArrCap);
		
		// The raw userinput is no longer needed, so free it
		free(userinput);
		
		struct FlopCommand *command = parse_flopsh(tokArr, nTokens);
		
		/* // This is the old version of the flopshell
		if (nCommandArgs == 0) {
			continue;
		}

		// If user quit
		if (strcmp(commandArgs[0], "quit") == 0 || strcmp(commandArgs[0], "q") == 0) {
			int i;
			for (i = 0; i < nCommandArgs; i++) {
				free(commandArgs[i]);
			}
			free(commandArgs);
			break;
		}

		// Find the shell command in the list of available commands
		const struct FlopCommand *command = get_shell_command(commandArgs[0]);
		if (command == NULL) {
			fprintf(stderr, "%s: command not found\n", commandArgs[0]);
			continue;
		}

		// Execute the command
		command->execute(&flopdata, nCommandArgs, commandArgs);
		*/
		// Clean up
		int i;
		for (i = 0; i < nTokens; i++) {
			free(tokArr[i]);
		}
		free(tokArr);
		free(userinput);
	}

	if (flopdata.rawData != NULL) {
		fumount(&flopdata);
	}
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

