#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "flopshell.h"
#include "FlopCommand.h"
#include "parse.h"
#include "exec_command.h"


static void flopshell_run();
static bool isQuitCommand(struct FlopCommand *);
static struct FlopShellState *FlopShellState_new();
static void FlopShellState_destroy(struct FlopShellState *);


static const char *SHELL_PROMPT = "flop: ";


// Start the flop shell
int main() {
	flopshell_run();
	return 0;
}


// Runs the flop shell until the user enters the "quit" command
static void flopshell_run() {

	struct FlopShellState *flopstate = FlopShellState_new();

	// Init user input buffer
	size_t inputBufCapacity = 256;
	char *userinput = calloc(inputBufCapacity, sizeof(*userinput));

	while (!flopstate->hasQuit) {
		printf("%s", SHELL_PROMPT);

		// Get command from user
		size_t inputlen = getline(&userinput, &inputBufCapacity, stdin);

		// Remove the newline character at the end of the buffer
		userinput[inputlen - 1] = '\0';

		// Parse the array of tokens into a command
		struct FlopCommand *command = parse_flopsh(userinput);

		// Check if the command was a quit command
		if (!flopstate->hasQuit && command != NULL) {
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


// Creates a FlopShellState
static struct FlopShellState *FlopShellState_new() {
	struct FlopShellState *state = calloc(1, sizeof(struct FlopShellState));


	// Malloc the PATH variable so it can be expanded later if necessary
	state->pathLen = 0;
	state->pathCap = 256;
	state->pathVar = calloc(state->pathCap, sizeof(char));
	state->pathVar[0] = '\0';

	state->hasQuit = false;

	return state;
}


// Destroys a FlopShellState
static void FlopShellState_destroy(struct FlopShellState *state) {
	free(state->pathVar);
	free(state);
}
