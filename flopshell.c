#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <termios.h>

#include "flopshell.h"
#include "FlopCommand.h"
#include "parse.h"
#include "exec_command.h"
#include "internalcommands.h"


static void flopshell_run();
static struct FlopShellState *FlopShellState_new();
static void FlopShellState_destroy(struct FlopShellState *);
static size_t getInputLine(char **, size_t*);


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
		size_t inputlen = getInputLine(&userinput, &inputBufCapacity);

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

	// Set the internal commands for the shell
	state->internalCmds = calloc(5, sizeof(struct InternalCommand));
	state->internalCmds[0] = (struct InternalCommand){"quit", command_quit};
	state->internalCmds[1] = (struct InternalCommand){"q", command_quit};
	state->internalCmds[2] = (struct InternalCommand){"path", command_path};
	state->internalCmds[3] = (struct InternalCommand){"cd", command_cd};
	state->internalCmds[4] = (struct InternalCommand){NULL, NULL}; // Null-terminate the array

	state->hasQuit = false;

	return state;
}


// Destroys a FlopShellState
static void FlopShellState_destroy(struct FlopShellState *state) {
	free(state->pathVar);
	free(state->internalCmds);
	free(state);
}


// Gets a line of input
static size_t getInputLine(char **buf, size_t *bufCap) {
	// Save the current terminal settings
	struct termios oldtermio;
	struct termios newtermio;
	tcgetattr(0, &oldtermio);
	
	// Set the terminal to unbuffered input mode
	newtermio = oldtermio;
	newtermio.c_lflag &= ~ECHO;
	newtermio.c_lflag &= ~ICANON;
	
	tcsetattr(0, TCSANOW, &newtermio);
	
	off_t bufPos = 0;
	size_t bufLen = 0;
	(*buf)[bufLen] = '\0';
	char ch;
	
	while(ch = getchar()) {
		if(*bufCap < bufLen + 2) {
			*buf = realloc(*buf, (*bufCap) * 2 * sizeof(**buf));
			(*bufCap) *= 2;
		}
		
		if(ch == 127 && 0 < bufPos) {
			bufPos--;
			bufLen--;
			(*buf)[bufLen] = '\0';
			printf("\b \b");
			continue;
		} else if (ch == 127) {
			continue;
		}
		
		(*buf)[bufPos++] = ch;
		bufLen++;
		(*buf)[bufLen] = '\0';
		printf("%c", ch);
		if(ch == '\n') {
			break;
		}
	}
	
	// Set the terminal configuration back to the old settings
	tcsetattr(0, TCSANOW, &oldtermio);
	
	return bufLen;
}
