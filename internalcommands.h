#ifndef INTERNAL_COMMANDS_H_
#define INTERNAL_COMMANDS_H_

#include "flopshell.h"
#include "FlopCommand.h"


int command_quit(struct FlopShellState *, struct FlopCommand *);
int command_cd(struct FlopShellState *, struct FlopCommand *);
int command_path(struct FlopShellState *, struct FlopCommand *);


#endif
