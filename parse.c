#include <stdlib.h>

#include "parse.h"

// Parse the given set of tokens into a command structure
struct FlopCommand *parse_flopsh(char **tokArr, int nTokens) {
	struct FlopCommand *command = FlopCommand_new();
	command->commandName = tokArr[0];
	
	return command;
}
