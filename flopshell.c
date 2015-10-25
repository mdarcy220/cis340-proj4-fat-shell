#include "flopshell.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef char** StringArray;
//typedef enum {FALSE=0, TRUE} bool;

static void flopshell_run();
static int quit(struct FlopData*, int, char**);
static size_t parse_command_args(char*, StringArray*, size_t*);
static void add_str_to_arr(char*, StringArray*, size_t, size_t*);
static const struct FlopCommand* get_shell_command(char*);

int fmount(struct FlopData*, int, char**);
int fumount(struct FlopData*, int, char**);

static const struct FlopCommand AVAILABLE_COMMANDS[] = {
	{"fmount", fmount},
	{"fumount", fumount},
	{0}
};

static const char *SHELL_PROMPT = "flop: ";


void flopshell_start() {
	flopshell_run();
}


// Runs the flop shell until the user enters the "quit" command
static void flopshell_run() {
	
	struct FlopData flopdata = {NULL, 0};
	
	//bool hasquit = FALSE;
	
	for(;;) {
		// Get command from user
		size_t inputBufCapacity = 256;
		char *userinput = malloc(inputBufCapacity*sizeof(*userinput));
		printf("%s", SHELL_PROMPT);
		int inputlen = getline(&userinput, &inputBufCapacity, stdin);
		
		// Remove the newline character at the end of the buffer
		userinput[inputlen-1] = '\0';
		
		if(strcmp(userinput, "quit") == 0) {
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
	}
}


// Quits the program
static int quit(struct FlopData* flopdata, int argc, char **argv) {
	// exit(0);
	
	return EXIT_SUCCESS;
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
		*arr = realloc(*arr, ++(*arrCap));
	}
	(*arr)[arrLen] = malloc(strlen(str)*sizeof(*str));
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


// Mounts a FAT12 img file into the FlopData's image data
int fmount(struct FlopData *flopdata, int argc, char **argv) {
	// TODO: Add checks to catch errors
	
	if(argc != 2) {
		fprintf(stderr, "Invalid number of arguments.\n");
		return EXIT_FAILURE;
	}
	
	if(flopdata->rawData != NULL) {
		fprintf(stderr, "Error. There is an image already mounted. Please unmount it using fumount and try again.\n");
		return EXIT_FAILURE;
	}
	
	flopdata->rawData = malloc((2880*512 + 1)*sizeof(*flopdata->rawData));
	
	int fd = open(argv[1], O_RDONLY);
	flopdata->rawDataLen = read(fd, flopdata->rawData, 2880*512);
	close(fd);
	
	return 0;
}


// Unmounts the given FlopData image data
int fumount(struct FlopData *flopdata, int argc, char **argv) {
	
	if(flopdata->rawData == NULL) {
		fprintf(stdout, "Warning: There was no image mounted.\n");
	}
	
	flopdata->rawData = NULL;
	flopdata->rawDataLen = 0;
	printf("The image was successfully unmounted.\n");
	return 0;
}
