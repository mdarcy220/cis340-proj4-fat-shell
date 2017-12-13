
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

#include "traverse.h"
#include "structure.h"
#include "showsector.h"
#include "showfile.h"
#include "showfat.h"
#include "help.h"
#include "fmountutils.h"

#include "flop.h"
#include <stdlib.h>


#define IMAGEFILE_PATH "imagefile.img"


#ifdef HELP_MAIN

int main(int argc, char **argv) {
	return command_help(NULL, argc, argv);
}

#endif


#ifdef SHOWFAT_MAIN

int main(int argc, char **argv) {
	struct FlopData *data = calloc(1, sizeof(struct FlopData));
	fmount(data, IMAGEFILE_PATH);
	return command_showfat(data, argc, argv);
}

#endif


#ifdef SHOWFILE_MAIN

int main(int argc, char **argv) {
	struct FlopData *data = calloc(1, sizeof(struct FlopData));
	fmount(data, IMAGEFILE_PATH);
	return command_showfile(data, argc, argv);
}

#endif


#ifdef SHOWSECTOR_MAIN

int main(int argc, char **argv) {
	struct FlopData *data = calloc(1, sizeof(struct FlopData));
	fmount(data, IMAGEFILE_PATH);
	return command_showsector(data, argc, argv);
}

#endif


#ifdef STRUCTURE_MAIN

int main(int argc, char **argv) {
	struct FlopData *data = calloc(1, sizeof(struct FlopData));
	fmount(data, IMAGEFILE_PATH);
	return print_fs_structure(data, argc, argv);
}

#endif


#ifdef TRAVERSE_MAIN

int main(int argc, char **argv) {
	struct FlopData *data = calloc(1, sizeof(struct FlopData));
	fmount(data, IMAGEFILE_PATH);
	return command_traverse(data, argc, argv);
}

#endif



