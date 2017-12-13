
/*
 * Copyright (C) 2017 Mike D'Arcy
 *
 * This file is part of FlopShell, a toy shell utility for floppy disk images.
 * 
 * FlopShell is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * FlopShell is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FlopShell.  If not, see <http://www.gnu.org/licenses/>.
 */

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
