#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "flop.h"
#include "fmountutils.h"

// Mounts a FAT12 img file into the FlopData's image data
int command_fmount(struct FlopData *flopdata, int argc, char **argv) {
	// TODO: Add checks to catch errors
	
	if(argc != 2) {
		fprintf(stderr, "Invalid number of arguments.\n");
		return EXIT_FAILURE;
	}
	
	if(flopdata->rawData != NULL) {
		fprintf(stderr, "Error. There is an image already mounted. Please unmount it using fumount and try again.\n");
		return EXIT_FAILURE;
	}
	
	flopdata->rawData = malloc((2880*512 + 1)*sizeof(*flopdata->rawData));
	
	int fd = open(argv[1], O_RDONLY);
	flopdata->rawDataLen = read(fd, flopdata->rawData, 2880*512);
	close(fd);
	
	return 0;
}


// Unmounts the given FlopData image data
int command_fumount(struct FlopData *flopdata, int argc, char **argv) {
	
	if(flopdata->rawData == NULL) {
		fprintf(stdout, "Warning: There was no image mounted.\n");
	}
	
	fumount(flopdata);
	
	printf("The image was successfully unmounted.\n");
	return 0;
}


// Unmount the given image data
int fumount(struct FlopData *flopdata) {
	free(flopdata->rawData);
	
	flopdata->rawData = NULL;
	flopdata->rawDataLen = 0;
}
