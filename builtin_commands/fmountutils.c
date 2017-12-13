
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "flop.h"
#include "fmountutils.h"
#include "structure.h"


/*typedef enum {
	MOUNT_SUCCESS,
	IMAGE_ALREADY_MOUNTED,
	FILE_OPEN_ERROR
} FMountStatus;*/


// Mounts a FAT12 img file into the FlopData's image data
int command_fmount(struct FlopData *flopdata, int argc, char **argv) {
	// TODO: Add checks to catch errors
	
	if(argc != 2) {
		fprintf(stderr, "Invalid number of arguments.\n");
		return EXIT_FAILURE;
	}
	
	return fmount(flopdata, argv[1]);
}


// Unmounts the given FlopData image data
int command_fumount(struct FlopData *flopdata, int argc, char **argv) {
	
	if(!has_mounted_image(flopdata)) {
		fprintf(stdout, "Warning: There was no image mounted.\n");
	}
	
	fumount(flopdata);
	
	printf("The image was successfully unmounted.\n");
	return EXIT_SUCCESS;
}


// Mounts the floppy image located at the given path
int fmount(struct FlopData *flopdata, char *imagePath) {
	if(has_mounted_image(flopdata)) {
		fprintf(stderr, "Error. There is an image already mounted. Please unmount it using fumount and try again.\n");
		return EXIT_FAILURE;
	}
	
	int fd = open(imagePath, O_RDONLY);
	if(fd == -1) {
		fprintf(stderr, "Error opening image file. The image was not mounted successfully.\n");
		return EXIT_FAILURE;
	}
	
	flopdata->rawData = malloc((2880*512 + 1)*sizeof(*flopdata->rawData));
	
	flopdata->rawDataLen = read(fd, flopdata->rawData, 2880*512);
	close(fd);
	
	load_fs_structure(flopdata);
	
	return EXIT_SUCCESS;
}


// Unmount the given image data
int fumount(struct FlopData *flopdata) {
	free(flopdata->rawData);
	
	flopdata->rawData = NULL;
	flopdata->rawDataLen = 0;
	flopdata->fatEntryWidth = 0;
	flopdata->nFatTables = 0;
	flopdata->sectorsPerFat = 0;
	flopdata->sectorsPerCluster = 0;
	flopdata->nRootEntries = -1;
	flopdata->bytesPerSector = 0;
	flopdata->nReservedSectors = -1;
	
	return 0;
}


// Checks if an image has been mounted into the given FlopData
int has_mounted_image(struct FlopData *flopdata) {
	return (0 < flopdata->rawDataLen && flopdata->rawData != NULL);
}
