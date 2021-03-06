
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

#include <stdlib.h>
#include <stdio.h>
#include "flop.h"
#include "structure.h"
#include "concatbytes.h"
#include "hexdump.h"
#include "fmountutils.h"


static const int DIRECTORY_ENTRY_SIZE = 32;


// Prints information about the structure of the filesystem
int print_fs_structure(struct FlopData *flopdata, int argc, char **argv) {

	if (!has_mounted_image(flopdata)) {
		fprintf(stderr, "Error. Unable to load FAT data. No image mounted.\n");
		return 1;
	}

	load_fs_structure(flopdata);


	printf("                number of FAT:                  %4d\n", flopdata->nFatTables);
	printf("                number of sectors used by FAT:  %4d\n", flopdata->sectorsPerFat);
	printf("                number of sectors per cluster:  %4d\n", flopdata->sectorsPerCluster);
	printf("                number of of ROOT entries:      %4d\n", flopdata->nRootEntries);
	printf("                number of bytes per sector:     %4d\n", flopdata->bytesPerSector);
	printf("                ---Sector #---     ---Sector Types---\n");

	if (flopdata->nReservedSectors == 1) {
		printf("                      0                  BOOT        \n");
	} else {
		printf("                    %02d -- %02d             BOOT        \n", 0,
		       flopdata->nReservedSectors - 1);
	}


	int fatNum;
	for (fatNum = 1; fatNum <= flopdata->nFatTables; fatNum++) {
		printf("                   %02d -- %02d              FAT%d        \n",
		       flopdata->nReservedSectors + (fatNum - 1) * flopdata->sectorsPerFat,
		       flopdata->nReservedSectors + fatNum * flopdata->sectorsPerFat - 1, fatNum);
	}

	printf("                   %02d -- %02d              ROOT DIRECTORY\n",
	       calc_root_start_sector(flopdata), calc_data_start_sector(flopdata) - 1);

	return 0;
}


// Loads information about the structure of the given FlopData image filesystem into the given FlopData. This
// method is deprecated. Please use load_fs_data instead.
int get_fs_structure(struct FlopData *flopdata) {

	flopdata->bytesPerSector = concat_uint8_uint16(flopdata->rawData[12], flopdata->rawData[11]);
	flopdata->sectorsPerCluster = (int)flopdata->rawData[13];
	flopdata->nReservedSectors = concat_uint8_uint16(flopdata->rawData[15], flopdata->rawData[14]);
	flopdata->nFatTables = (int)flopdata->rawData[16];
	flopdata->nRootEntries = concat_uint8_uint16(flopdata->rawData[18], flopdata->rawData[17]);
	flopdata->sectorsPerFat = (int)flopdata->rawData[22];


	return 0;
}


// Loads information about the structure of the given FlopData image filesystem into the given FlopData
int load_fs_structure(struct FlopData *flopdata) {

	if (!has_mounted_image(flopdata)) {
		fprintf(stderr, "Error. No image mounted.");
		return 1;
	}

	flopdata->bytesPerSector = concat_uint8_uint16(flopdata->rawData[12], flopdata->rawData[11]);
	flopdata->sectorsPerCluster = (int)flopdata->rawData[13];
	flopdata->nReservedSectors = concat_uint8_uint16(flopdata->rawData[15], flopdata->rawData[14]);
	flopdata->nFatTables = (int)flopdata->rawData[16];
	flopdata->nRootEntries = concat_uint8_uint16(flopdata->rawData[18], flopdata->rawData[17]);
	flopdata->sectorsPerFat = (int)flopdata->rawData[22];


	return 0;
}


// Gets the number of sectors preceeding the root sector
// Warning: This function assumes the filesystem structure has already been loaded into the FlopData
int calc_root_start_sector(struct FlopData *flopdata) {
	return flopdata->nReservedSectors + (flopdata->nFatTables) * (flopdata->sectorsPerFat);
}


// Gets the number of sectors preceeding the root sector
// Warning: This function assumes the filesystem structure has already been loaded into the FlopData
int calc_data_start_sector(struct FlopData *flopdata) {
	return flopdata->nReservedSectors + (flopdata->nFatTables) * (flopdata->sectorsPerFat) +
			((DIRECTORY_ENTRY_SIZE * flopdata->nRootEntries) / flopdata->bytesPerSector);
}
