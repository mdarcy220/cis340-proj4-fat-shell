#include <stdlib.h>
#include <stdio.h>
#include "flop.h"
#include "structure.h"


// Prints information about the structure of the filesystem
int print_fs_structure(struct FlopData *flopdata, int argc, char **argv) {
	get_fs_structure(flopdata);
	return 0;
}


// Loads information about the structure of the given FlopData image filesystem into the given FlopData
int get_fs_structure(struct FlopData *flopdata) {
	if(flopdata->rawData == NULL) {
		fprintf(stderr, "Error. Unable to load FAT. No image mounted.\n");
		return 1;
	}
	
	flopdata->sectorSize = ((short) flopdata->rawData[11]) | ( ((short) flopdata->rawData[12]) << 8 );
	printf("sectorSize = %d\n", flopdata->sectorSize);
	
	return 0;
}
