#include "flop.h"
#include "showsector.h"
#include "fmountutils.h"
#include "structure.h"
#include "hexdump.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int command_showsector(struct FlopData *flopdata, int argc, char **argv) {
	
	if(argc != 2) {
		fprintf(stderr, "Error. Invalid number of arguments. Please specify a sector number.\n");
		return 1;
	}
	
	unsigned short sec = atoi(argv[1]);

	if(!has_mounted_image(flopdata)) {
		fprintf(stderr, "Error. No image mounted.\n");
		return 1;
	}
	
	if(load_fs_structure(flopdata) != 0) {
		fprintf(stderr, "Error. Filesystem structure data could not be loaded.\n");
		return 1;
	}

	off_t offset = flopdata->bytesPerSector * sec;

	print_hex_dump((flopdata->rawData + offset), flopdata->bytesPerSector);
	fprintf(stdout, "\n");
	return 0;
}
