#ifndef FLOPSHELL_H_
#define FLOPSHELL_H_

#include <stdlib.h>
#include "flop.h"

// Represents a shell command
struct FlopCommand {
	char *commandName;
	int (*execute)(struct FlopData* flopdata, int argc, char **argv);
};


void flopshell_start();

#endif
