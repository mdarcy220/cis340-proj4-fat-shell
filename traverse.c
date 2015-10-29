#include <stdlib.h>
#include <stdio.h>

#include "flop.h"
#include "concatbytes.h"
#include "hexdump.h"
#include "fmountutils.h"

// Print all entries in the ROOT table
int command_traverse(struct FlopData *flopdata, int argc, char **argv) {
	if(!has_mounted_image(flopdata)) {
		fprintf(stderr, "Error. Unable to load FAT data. No image mounted.\n");
		return 1;
	}
	
	flopdata->rawData; //to get the rawdata from img file
	

	return 0;

}
