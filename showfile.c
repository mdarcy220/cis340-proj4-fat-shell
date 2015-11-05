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
static int enter_subdirectory(struct FlopData *, struct rootent *, char **);
static size_t get_next_filename(char *, char *, size_t, off_t);
static int cluster2sector(struct FlopData *, int);


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
	char *path = argv[1];
	size_t pathLen = strlen(path);
	char curFilename[13];
	curFilename[0] = '\0';
	if (path[0] != '/') {
		fprintf(stderr, "Error. Only absolute paths are supported for the showfile command. Please "
				"make sure you enter a path that starts with a '/')\n");
		return 1;
	}
	off_t pathIndex = 1;
	pathIndex += get_next_filename(curFilename, path, pathLen, pathIndex) + 1;

	// Initially get directory entries from root directory
	char **dirEntriesData = malloc(flopdata->nRootEntries * ROOT_ENTRY_SIZE * sizeof(char *));
	dirEntriesData[0] = flopdata->rawData + calc_root_start_sector(flopdata) * flopdata->bytesPerSector;
	int entryNum = 0;
	struct rootent *curEnt = malloc(sizeof(*curEnt));
	char *curEntData;
	int entriesPerSector = flopdata->bytesPerSector / ROOT_ENTRY_SIZE;
	while ((curEntData = dirEntriesData[entryNum * ROOT_ENTRY_SIZE % flopdata->bytesPerSector] +
				(entryNum * ROOT_ENTRY_SIZE % flopdata->bytesPerSector))[0] != 0) {
		parse_rootent(curEntData, curEnt);

		char filename_full[13];
		filename_full[0] = '\0';
		strcpy(filename_full, curEnt->filename);
		if (strcmp(curEnt->fileext, "") != 0) {
			strcat(filename_full, ".");
			strcat(filename_full, curEnt->fileext);
		}

		if (strcasecmp(filename_full, curFilename) == 0) {
			size_t tmp = 0;
			if (0 < (tmp = get_next_filename(curFilename, path, pathLen, pathIndex))) {
				pathIndex += tmp;
				enter_subdirectory(flopdata, curEnt,
						   dirEntriesData + (entryNum * ROOT_ENTRY_SIZE %
								     flopdata->bytesPerSector));
				entryNum = 0;
				continue;
			} else {
				print_file_dump(flopdata, curEnt);
				foundFile = 1;
				break;
			}
		}

		entryNum++;
	}
	free(curEnt);

	if (!foundFile) {
		fprintf(stderr, "File not found.\n");
	}

	return 0;
}


// Prints a file starting at the given cluster as a hex dump
static int print_file_dump(struct FlopData *flopdata, struct rootent *fileEnt) {
	int *sectors = malloc(flopdata->sectorsPerCluster * 5 * sizeof(int));
	int nSectors = 0;
	char *fatData = flopdata->rawData + (flopdata->nReservedSectors * flopdata->bytesPerSector);

	unsigned int nextCluster = fileEnt->first_cluster;
	while (nextCluster < 0xFF8) {
		if (nSectors % (flopdata->sectorsPerCluster * 5) == 0 && nSectors != 0) {
			sectors = realloc(sectors, (flopdata->sectorsPerCluster * 5 + 5) * sizeof(int));
		}
		int i = 0;
		for (i = 0; i < flopdata->sectorsPerCluster; i++) {
			sectors[nSectors++] = cluster2sector(flopdata, nextCluster) + i;
		}


		char *entry = fatData + ((nextCluster * 12) / 8);
		if (nextCluster % 2 == 0) {
			nextCluster = concat_uint8_uint16(entry[1] & 0x0F, entry[0]);
		} else {
			nextCluster = concat_uint8_uint16(entry[1], entry[0] & 0xF0);
		}
	}

	show_sectors(flopdata, sectors, nSectors);

	free(sectors);

	return 0;
}


// Set the given data pointer to point to the directory data given by the rootent
static int enter_subdirectory(struct FlopData *flopdata, struct rootent *dirEntry, char **dataPointer) {
	*dataPointer = flopdata->rawData +
			cluster2sector(flopdata, dirEntry->first_cluster) * flopdata->bytesPerSector;
	return 0;
}


// Gets the substring of path starting at the given index and ending at the next '/', stores it into the
// given buffer and returns the number of characters stored in buf (excluding null terminator, -1 on error).
// The trailing '/' is not
// stored.
static size_t get_next_filename(char *buf, char *path, size_t pathLength, off_t pathIndex) {
	off_t tmpIndex = pathIndex;
	size_t nRead = 0;
	while (tmpIndex < pathLength && path[tmpIndex] != '/') {
		buf[nRead++] = path[tmpIndex++];
	}
	if (buf[nRead - 1] != '\0') {
		buf[nRead] = '\0';
	}

	return nRead;
}


// Converts the given cluster number to the corresponding sector number, and returns the sector number
static int cluster2sector(struct FlopData *flopdata, int clusterNum) {
	return clusterNum * flopdata->sectorsPerCluster + calc_data_start_sector(flopdata) - 2;
}
