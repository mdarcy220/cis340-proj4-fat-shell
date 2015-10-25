#ifndef FLOPSHELL_H_
#define FLOPSHELL_H_

#include <stdlib.h>

struct FlopData {
	char * rawData;
	size_t rawDataLen;
};

struct FlopCommand {
	char *commandName;
	int (*execute)(struct FlopData* flopdata, int argc, char **argv);
};


void flopshell_start();

#endif
