#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "flopshell.h"
#include "flop.h"

// include shell command functions
#include "fmountutils.h"
#include "showfat.h"
#include "showfile.h"
#include "showsector.h"
#include "structure.h"
#include "traverse.h"


typedef char** StringArray;


static void flopshell_run();
static size_t parse_command_args(char*, StringArray*, size_t*);
static void add_str_to_arr(char*, StringArray*, size_t, size_t*);
static const struct FlopCommand* get_shell_command(char*);


static const char *SHELL_PROMPT = "flop: ";

// Commands that can be executed
static const struct FlopCommand AVAILABLE_COMMANDS[] = {
	{"fmount", fmount},
	{"fumount", fumount},
	{"showfat", showfat},
	{"showfile", showfile},
	{"showsector", showsector},
	{"structure", structure},
	{"traverse", traverse},
	{0}
};


// Start the flop shell
void flopshell_start() {
	flopshell_run();
}


// Runs the flop shell until the user enters the "quit" command
static void flopshell_run() {
	
	struct FlopData flopdata = {NULL, 0};
	
	for(;;) {
		// Get command from user
		size_t inputBufCapacity = 256;
		char *userinput = malloc(inputBufCapacity*sizeof(*userinput));
		printf("%s", SHELL_PROMPT);
		int inputlen = getline(&userinput, &inputBufCapacity, stdin);
		
		// Remove the newline character at the end of the buffer
		userinput[inputlen-1] = '\0';
		
		if(strcmp(userinput, "quit") == 0) {
			free(userinput);
			break;
		}
		
		// Parse the input into a list of command arguments
		size_t commandArgsCap = 1;
		StringArray commandArgs  = malloc(commandArgsCap*sizeof(char*));
		size_t nCommandArgs = parse_command_args(userinput, &commandArgs, &commandArgsCap);

		if(nCommandArgs == 0) {
			continue;
		}

		// Find the shell command in the list of available commands
		const struct FlopCommand *command = get_shell_command(commandArgs[0]);
		if(command == NULL) {
			fprintf(stderr, "%s: command not found\n", commandArgs[0]);
			continue;
		}
		
		// Execute the command
		command->execute(&flopdata, nCommandArgs, commandArgs);
		
		// Clean up
		int i;
		for(i = 0; i < nCommandArgs; i++) {
			free(commandArgs[i]);
		}
		free(commandArgs);
		free(userinput);
	}
	
	free(flopdata.rawData);
}


/* 
 * Parses a command string into an array of arguments stored in the StringArray given by argsArr, 
 * and returns number of arguments found. If the capacity of argsArr is not sufficient to hold all 
 * the arguments, it will be extended using realloc, and the new capacity will be stored in 
 * argsArrCapacity.
 */
static size_t parse_command_args(char* commandStr, StringArray *argsArr, size_t *argsArrCapacity) {
	if(commandStr == NULL || argsArr == NULL || argsArrCapacity == NULL || commandStr[0] == '\0') {
		return 0;
	}
	
	size_t nArgs = 0;
	
	char delim = ' ';
	
	char curArg[256];
	int curArgLen = 0;
	
	int index;
	for(index = 0; index < strlen(commandStr)+1; index++) {
		if ( (commandStr[index] == delim || commandStr[index] == '\0') && curArgLen != 0 ) {
			curArg[curArgLen++] = '\0';
			add_str_to_arr(curArg, argsArr, nArgs, argsArrCapacity);
			nArgs++;
			curArgLen = 0;
		} else {
			curArg[curArgLen++] = commandStr[index];
		}
	}
	
	return nArgs;
}


// Copies the given string into the given buffer, expanding the buffer using realloc if necessary
static void add_str_to_arr(char *str, StringArray *arr, size_t arrLen, size_t *arrCap) {
	if(*arrCap < arrLen+1) {
		(*arrCap)++;
		*arr = realloc(*arr, (*arrCap)*sizeof(**arr));
	}
	(*arr)[arrLen] = malloc((strlen(str) + 1)*sizeof(*str));
	strcpy((*arr)[arrLen], str);
}


// Get the appropriate shell command from its name
static const struct FlopCommand* get_shell_command(char *commandName) {
	int i;
	for(i = 0; AVAILABLE_COMMANDS[i].commandName != 0; i++) {
		if(strcmp(commandName, AVAILABLE_COMMANDS[i].commandName) == 0) {
			return &AVAILABLE_COMMANDS[i];
		}
	}
	
	return NULL;
}
