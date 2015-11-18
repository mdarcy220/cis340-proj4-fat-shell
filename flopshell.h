#ifndef FLOPSHELL_H_
#define FLOPSHELL_H_

#include <stdlib.h>
#include "FlopCommand.h"

typedef enum { false = 0, true } bool;

struct FlopShellState;
struct InternalCommand;

struct InternalCommand {
	char *commandName;
	int (*execute)(struct FlopShellState *, struct FlopCommand *);
};

struct FlopShellState {
	char* pathVar;
	size_t pathCap;
	size_t pathLen;
	struct InternalCommand *internalCmds;
	bool hasQuit;
};

void flopshell_start();

#endif
