
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "showfat.h"
#include "flop.h"
#include "fmountutils.h"
#include "structure.h"


// Displays the first 256 entries of the FAT
int command_showfat(struct FlopData *flopdata, int argc, char **argv) {

	unsigned short fatBytes = 256 * 3 / 2; // Show the first 256 entries
	unsigned short cluster;
	char *fatBuffer;

	if (!has_mounted_image(flopdata)) {
		fprintf(stderr, "Error. No image mounted.\n");
		return 1;
	}

	if (load_fs_structure(flopdata) != 0) {
		fprintf(stderr, "Error. Filesystem structure data could not be loaded.\n");
		return 1;
	}

	off_t offset;

	offset = (flopdata->nReservedSectors * flopdata->bytesPerSector);
	if (flopdata->rawDataLen < (offset + fatBytes)) {
		fprintf(stderr, "Error. Missing FAT data.\n");
		return 1;
	}
	fatBuffer = flopdata->rawData + offset;

	// set up horizontal hex values
	fprintf(stdout, "     ");
	int i;
	for (i = 0; i < 16; i++) {
		fprintf(stdout, " %4x", i);
	}

	fprintf(stdout, "\n      %4s %4s", "", ""); // first two entries are reserved
	int x;
	for (x = 2; x < fatBytes * 2 / 3; x++) {
		unsigned short low, high;
		unsigned short temp = (unsigned short)x;
		// prints the vertical hex values
		if ((x % 16) == 0) {
			fprintf(stdout, "\n %4x", x);
		}

		if (temp % 2) {
			low = (((unsigned short)fatBuffer[(3 * temp - 1) / 2]) >> 4) & 0x000f;
			high = (((unsigned short)fatBuffer[(3 * temp + 1) / 2]) << 4) & 0x0ff0;
		} else {
			low = ((unsigned short)fatBuffer[3 * temp / 2]) & 0x00ff;
			high = (((unsigned short)fatBuffer[(3 * temp + 2) / 2]) << 8) & 0x0f00;
		}
		cluster = low | high;

		if (cluster) {
			fprintf(stdout, " %4x", cluster);
		} else {
			fprintf(stdout, " FREE");
		}
	}

	fprintf(stdout, "\n");
	return 0;
}
