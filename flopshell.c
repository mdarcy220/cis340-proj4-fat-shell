#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "flopshell.h"
#include "FlopCommand.h"
#include "parse.h"
#include "exec_command.h"
#include "flop.h"
#include "fmountutils.h"


typedef enum { false = 0, true } bool;


static void flopshell_run();
static bool isQuitCommand(struct FlopCommand *);
static struct FlopShellState *FlopShellState_new();
static void FlopShellState_destroy(struct FlopShellState *);


static const char *SHELL_PROMPT = "flop: ";


// Start the flop shell
void flopshell_start() { flopshell_run(); }


// Runs the flop shell until the user enters the "quit" command
static void flopshell_run() {

	struct FlopShellState *flopstate = FlopShellState_new();

	bool userQuit = false;

	// Init user input buffer
	size_t inputBufCapacity = 256;
	char *userinput = calloc(inputBufCapacity, sizeof(*userinput));

	while (!userQuit) {
		printf("%s", SHELL_PROMPT);

		// Get command from user
		size_t inputlen = getline(&userinput, &inputBufCapacity, stdin);

		// Remove the newline character at the end of the buffer
		userinput[inputlen - 1] = '\0';

		// Parse the array of tokens into a command
		struct FlopCommand *command = parse_flopsh(userinput);

		// Check if the command was a quit command
		userQuit = isQuitCommand(command);
		if (!userQuit && command != NULL) {
			// Execute the command
			exec_command(flopstate, command);
		}

		// Destroy the command and free its memory
		FlopCommand_destroy(command);
	}

	// Clean up
	FlopShellState_destroy(flopstate);
	free(userinput);
}


// Checks if the given command is used to quit the shell
static bool isQuitCommand(struct FlopCommand *command) {
	if (command == NULL) {
		return false;
	}
	return strcmp(command->commandName, "quit") == 0 || strcmp(command->commandName, "q") == 0;
}


// Creates a FlopShellState
static struct FlopShellState *FlopShellState_new() {
	struct FlopShellState *state = malloc(sizeof(struct FlopShellState));
	memset(state, 0, sizeof(struct FlopShellState));

	// Malloc the PATH variable so it can be expanded later if necessary
	state->pathVar = malloc(256 * sizeof(char));
	state->pathVar[0] = '\0';


	state->flopdata = malloc(sizeof(struct FlopData));
	memset(state->flopdata, 0, sizeof(struct FlopData));
	fmount(state->flopdata, "./imagefile.img");

	return state;
}


// Destroys a FlopShellState
static void FlopShellState_destroy(struct FlopShellState *state) {
	free(state->pathVar);
	fumount(state->flopdata);
	free(state->flopdata);
	free(state);
}
