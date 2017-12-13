
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

#include <stdio.h>
#include "help.h"


// The help shell command
int command_help(struct FlopData *flopdata, int argc, char **argv) {
	print_help();
	return 0;
}


// Print help
void print_help() {
	
        printf("\nThe shell has the following commands.\n\n");
        printf("usage___________________________|result_______________________\n");
        printf("cd [directory] \t\t\t|change current directory.\n");
        printf("path [+/-] [pathname] \t\t|shows working path or appends current search path\n");             
        printf("showfat\t\t\t\t|shows the fat1 table for mounted image.\n");
        printf("traverse\t\t\t|shows directory entries on image.\n");
        printf("showfile [directory entry]\t|shows file raw data.\n");
	printf("structure\t\t\t|shows structure of disk.\n");
	printf("showsector [sector #]\t\t|show specified sector #.\n");
        printf("quit or q\t\t\t|texit out of the shell.\n\n");

}
