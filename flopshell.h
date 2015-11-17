#ifndef FLOPSHELL_H_
#define FLOPSHELL_H_

#include <stdlib.h>
#include "flop.h"

typedef enum { false = 0, true } bool;

struct FlopShellState {
	char* pathVar;
	size_t pathCap;
	size_t pathLen;
	bool hasQuit;
};

void flopshell_start();

#endif
