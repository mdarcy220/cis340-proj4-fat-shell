
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

#include "flop.h"
#include "showsector.h"
#include "fmountutils.h"
#include "structure.h"
#include "hexdump.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


// Prints a hex dump of the sector
int command_showsector(struct FlopData *flopdata, int argc, char **argv) {

	if(argc != 2) {
		fprintf(stderr, "Error. Invalid number of arguments. Please specify a sector number.\n");
		return 1;
	}

	int sec = atoi(argv[1]);
	return show_sectors(flopdata, &sec, 1);
}


// Print a hex dump of all the given sectors in order
int show_sectors(struct FlopData *flopdata, int *sectornums, int nSectors) {
	if(!has_mounted_image(flopdata)) {
		fprintf(stderr, "Error. No image mounted.\n");
		return 1;
	}

	if(load_fs_structure(flopdata) != 0) {
		fprintf(stderr, "Error. Filesystem structure data could not be loaded.\n");
		return 1;
	}

	if(0 < nSectors) {
		print_hex_dump(flopdata->rawData + (flopdata->bytesPerSector * sectornums[0]), flopdata->bytesPerSector);
	}

	int i;
	for(i = 1; i < nSectors; ++i) {
		off_t offset = flopdata->bytesPerSector * sectornums[i];
		append_hex_dump((flopdata->rawData + offset), flopdata->bytesPerSector, i*flopdata->bytesPerSector);
	}
	fprintf(stdout, "\n");

	return 0;
}
