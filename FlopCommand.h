#ifndef FLOP_COMMAND_H_
#define FLOP_COMMAND_H_

// Represents a shell command
struct FlopCommand {
	char *commandName;
	int argc;
	char **argv;
	char *inputFile;
	char *outputFile;
	struct FlopCommand *pipeCommand;
};

void FlopCommand_destroy(struct FlopCommand*);
struct FlopCommand *FlopCommand_new();

#endif
