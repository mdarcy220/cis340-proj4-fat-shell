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