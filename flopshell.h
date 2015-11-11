#ifndef FLOPSHELL_H_
#define FLOPSHELL_H_

#include <stdlib.h>
#include "flop.h"

struct FlopShellState {
	char* pathVar;
	struct FlopData *flopdata;
};

void flopshell_start();

#endif
