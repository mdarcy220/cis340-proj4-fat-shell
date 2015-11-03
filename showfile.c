#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "flop.h"
#include "fmountutils.h"
#include "structure.h"
#include "fatroot.h"
#include "showsector.h"
#include "concatbytes.h"

static int print_file_dump(struct FlopData *, struct rootent *);

// Shows a hex dump of the file given in argv
int command_showfile(struct FlopData *flopdata, int argc, char **argv) {

	static const int ROOT_ENTRY_SIZE = 32;

	if (argc != 2) {
		fprintf(stderr, "Invalid number of arguments.\nUsage: showfile [filename]\n");
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

	int foundFile = 0;
	char *filename = argv[1];
	char *rootDir = flopdata->rawData + calc_root_start_sector(flopdata) * flopdata->bytesPerSector;
	int i;
	for (i = 0; i < flopdata->nRootEntries; i++) {
		char *curEntData = rootDir + i * ROOT_ENTRY_SIZE;
		struct rootent *curEnt = malloc(sizeof(*curEnt));
		parse_rootent(curEntData, curEnt);

		char filename_full[13];
		filename_full[0] = '\0';
		strcpy(filename_full, curEnt->filename);
		strcat(filename_full, ".");
		strcat(filename_full, curEnt->fileext);

		if (strcasecmp(filename_full, filename) == 0) {
			print_file_dump(flopdata, curEnt);
			foundFile = 1;
			break;
		}

		free(curEnt);
	}

	if (!foundFile) {
		fprintf(stderr, "File not found.\n");
	}

	return 0;
}

// Prints a file starting at the given cluster as a hex dump
static int print_file_dump(struct FlopData *flopdata, struct rootent *fileEnt) {
	int *clusters = malloc(5 * sizeof(int));
	int nClusters = 0;
	char *fatData = flopdata->rawData + (flopdata->nReservedSectors * flopdata->bytesPerSector);

	int data_clusters_offset =
	    calc_root_start_sector(flopdata) + ((32 * flopdata->nRootEntries) / flopdata->bytesPerSector) - 2;

	unsigned int nextCluster = fileEnt->first_cluster;
	while (nextCluster < 0xFF8) {
		if (nClusters % 5 == 0 && nClusters != 0) {
			clusters = realloc(clusters, (nClusters + 5) * sizeof(int));
		}
		clusters[nClusters++] = data_clusters_offset + nextCluster;

		char *entry = fatData + ((nextCluster * 12) / 8);
		printf("%d : %lu\n", nextCluster, data_clusters_offset);
		if (nextCluster % 2 == 0) {
			nextCluster = concat_uint8_uint16(entry[1] & 0x0F, entry[0]);
		} else {
			nextCluster = concat_uint8_uint16(entry[1], entry[0] & 0xF0);
		}
		sleep(1);
	}

	show_sectors(flopdata, clusters, nClusters);

	free(clusters);

	return 0;
}
