#ifndef FLOP_COMMAND_H_
#define FLOP_COMMAND_H_


// Types of commands we can have
typedef enum CommandType {
	CommandType_Unknown,
	CommandType_Builtin,
	CommandType_Internal,
	CommandType_Normal
	
} CommandType;


// Represents a shell command
struct FlopCommand {
	char *commandName;
	int argc;
	char **argv;
	char *inputFile;
	char *outputFile;
	struct FlopCommand *pipeCommand;
	CommandType type;
};

void FlopCommand_destroy(struct FlopCommand*);
struct FlopCommand *FlopCommand_new();

#endif
