#include <stdlib.h>
#include <stdio.h>

#include "flop.h"
#include "concatbytes.h"
#include "hexdump.h"
#include "fmountutils.h"
#include "DirEntryIterator.h"

// Print all entries in the ROOT table
int command_traverse(struct FlopData *flopdata, int argc, char **argv) {
	if(!has_mounted_image(flopdata)) {
		fprintf(stderr, "Error. Unable to load FAT data. No image mounted.\n");
		return 1;
	}
	
	if (!has_mounted_image(flopdata)) {
		fprintf(stderr, "Error. No image mounted.\n");
		return 1;
	}

	if (load_fs_structure(flopdata) != 0) {
		fprintf(stderr, "Error. Filesystem structure data could not be loaded.\n");
		return 1;
	}
	
	DirEntryIterator *dirIterStack;
	
	struct DirEntryIterator *curIter = new_DirEntryIterator(flopdata, NULL);
	struct rootent *curEnt = malloc(sizeof(*curEnt));
	while (DirEntryIterator_next(curIter, curEnt) != NULL) {
		if(is_dir(curEnt)) {
			
		}
	}
	free(curEnt);
	destroy_DirEntryIterator(curIter);

	return 0;

}
