#include <stdlib.h>
#include <stdio.h>

#include "flop.h"
#include "fmountutils.h"
#include "structure.h"
#include "fatroot.h"

int command_showfile(struct FlopData *flopdata, int argc, char **argv) {

	static const int ROOT_ENTRY_SIZE = 32;

	if(argc != 2) {
		fprintf(stderr, "Invalid number of arguments.\nUsage: showfile [filename]\n");
		return 1;
	}

	if(!has_mounted_image(flopdata)) {
		fprintf(stderr, "Error. No image mounted.\n");
		return 1;
	}
	
	if(load_fs_structure(flopdata) != 0) {
		fprintf(stderr, "Error. Filesystem structure data could not be loaded.\n");
		return 1;
	}
	
	int i;
	for(i = 0; i < flopdata->nRootEntries; i++) {
		char *curEntData = flopdata->rawData + calc_root_start_sector(flopdata)*flopdata->bytesPerSector + i*ROOT_ENTRY_SIZE;
		struct rootent *curEnt = malloc(sizeof(*curEnt));
		parse_rootent(curEntData, curEnt);
		
		free(curEnt);
	}
	
	return 0;
}
