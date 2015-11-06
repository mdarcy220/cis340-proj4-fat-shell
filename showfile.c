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


struct DirEntryIterator {
	char **dirEntSectors;
	size_t dirEntSectorsCap;
	int entryNum;
	int sectorNum;

	int DIR_ENTRY_SIZE;
	int ENTRIES_PER_SECTOR;
};


static void print_file_dump(struct FlopData *, struct rootent *);
static int filename_matches(struct rootent *, char *);
static void load_directory_sectors(struct FlopData *, struct rootent *, char ***, size_t *);
static int get_file_sectors(struct FlopData *, struct rootent *, int **);
static size_t get_next_filename(char *, char *, off_t);
static int cluster2sector(struct FlopData *, int);

static struct DirEntryIterator *new_DirEntryIterator(struct FlopData *, struct rootent *);
static void destroy_DirEntrYIterator(struct DirEntryIterator *);
static struct rootent *DirEntryIterator_next(struct DirEntryIterator *, struct rootent *);


// Shows a hex dump of the file given in argv
int command_showfile(struct FlopData *flopdata, int argc, char **argv) {

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


	char *path = argv[1];
	if (path[0] != '/') {
		fprintf(stderr, "Error. Only absolute paths are supported for the showfile command. Please "
				"make sure you enter a path that starts with a '/')\n");
		return 1;
	}

	// Create a buffer to hold the current filename
	char curFilename[13];
	curFilename[0] = '\0';

	// Load the first part of the path into the buffer for the current filename
	off_t pathIndex = 1;
	pathIndex += get_next_filename(curFilename, path, pathIndex) + 1;

	int foundFile = 0;

	struct DirEntryIterator *dirIter = new_DirEntryIterator(flopdata, NULL);
	struct rootent *curEnt = malloc(sizeof(*curEnt));
	while (DirEntryIterator_next(dirIter, curEnt) != NULL) {
		if (!filename_matches(curEnt, curFilename)) {
			continue;
		}

		// Filename matched

		// Check if this was the last piece of the path, and if so print the file
		if (path[pathIndex - 1] == '\0' || path[pathIndex] == 0) {
			print_file_dump(flopdata, curEnt);
			foundFile = 1;
			break;
		} else {
			// Enter the subdirectory
			pathIndex += get_next_filename(curFilename, path, pathIndex) + 1;
			destroy_DirEntrYIterator(dirIter);
			dirIter = new_DirEntryIterator(flopdata, curEnt);
			if (dirIter == NULL) {
				printf("Oh no! : %s\n", curEnt->filename);
			}
			continue;
		}
	}
	free(curEnt);
	destroy_DirEntrYIterator(dirIter);

	if (!foundFile) {
		fprintf(stderr, "File not found.\n");
	}

	return 0;
}


// Prints the file given by ent as a hex dump
static void print_file_dump(struct FlopData *flopdata, struct rootent *ent) {
	int *sectors = 0;
	int nSectors = get_file_sectors(flopdata, ent, &sectors);
	show_sectors(flopdata, sectors, nSectors);
	free(sectors);
}


// Checks if the given filename matches (ignoring case) the one found in the given struct rootent
static int filename_matches(struct rootent *ent, char *filename) {
	char filename_full[13];
	filename_full[0] = '\0';
	strcpy(filename_full, ent->filename);
	if (strcmp(ent->fileext, "") != 0) {
		strcat(filename_full, ".");
		strcat(filename_full, ent->fileext);
	}

	return strcasecmp(filename_full, filename) == 0;
}


/*
 * This function finds all the sectors associated with the given struct rootent and stores them in the array
 * of char pointers that is pointed by dirEntSectors.
 */
static void load_directory_sectors(struct FlopData *flopdata, struct rootent *curEnt, char ***dirEntSectors,
				   size_t *dirEntSectorsCap) {
	int *sectors = 0;
	int nSectors = get_file_sectors(flopdata, curEnt, &sectors);
	int i;
	for (i = 0; i < nSectors; ++i) {
		if (i == *dirEntSectorsCap) {
			*dirEntSectors = realloc(*dirEntSectors, *dirEntSectorsCap += sizeof(char *));
		}
		(*dirEntSectors)[i] = flopdata->rawData + sectors[i] * flopdata->bytesPerSector;
	}
	free(sectors);
}


// Prints a file starting at the given cluster as a hex dump
static int get_file_sectors(struct FlopData *flopdata, struct rootent *fileEnt, int **sectors) {
	// We will allocate *sectors ourselves, so free if it was alloced before
	free(*sectors);

	// Start with space for one sector
	*sectors = malloc(flopdata->sectorsPerCluster * sizeof(int));
	int nSectors = 0;
	char *fatData = flopdata->rawData + (flopdata->nReservedSectors * flopdata->bytesPerSector);

	unsigned int nextCluster = fileEnt->first_cluster;
	while (nextCluster < 0xFF8) {
		// Allocate space for the next cluster
		*sectors = realloc(*sectors, (flopdata->sectorsPerCluster + nSectors) * sizeof(int));

		// Put in consecutive sectors (important when there is more than one sector per cluster)
		int i;
		for (i = 0; i < flopdata->sectorsPerCluster; i++) {
			(*sectors)[nSectors++] = cluster2sector(flopdata, nextCluster) + i;
		}

		// Find the next cluster from the FAT
		char *entry = fatData + ((nextCluster * 12) / 8);
		if (nextCluster % 2 == 0) {
			nextCluster = concat_uint8_uint16(entry[1] & 0x0F, entry[0]);
		} else {
			nextCluster = concat_uint8_uint16(entry[1], entry[0] & 0xF0);
		}
	}

	return nSectors;
}


// Gets the substring of path starting at the given index and ending at the next '/', stores it into the
// given buffer and returns the number of characters stored in buf (excluding null terminator, -1 on error).
// The trailing '/' is not
// stored.
static size_t get_next_filename(char *buf, char *path, off_t pathIndex) {
	off_t tmpIndex = pathIndex;
	size_t nRead = 0;
	while (path[tmpIndex] != '\0' && path[tmpIndex] != '/') {
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


/*
 * Creates a new DirEntryIterator and returns it (it and many of its members are malloc()'ed, and it should
 * only be deconstructed using destroy_DirEntrYIterator; don't free() it manually). The iterator will be set
 * to iterate over the given directory, unless the given directory is NULL, in which case it will iteratoe
 * over the ROOT directory. Returns NULL if the given rootent isn't a subdirectory.
 */
static struct DirEntryIterator *new_DirEntryIterator(struct FlopData *flopdata, struct rootent *directory) {
	struct DirEntryIterator *dirIter = malloc(sizeof(struct DirEntryIterator));

	// Init basic values
	dirIter->DIR_ENTRY_SIZE = 32;
	dirIter->ENTRIES_PER_SECTOR = flopdata->bytesPerSector / dirIter->DIR_ENTRY_SIZE;
	dirIter->entryNum = 0;
	dirIter->sectorNum = 0;

	if (directory == NULL) {
		// Initially get directory entries from root directory, so load all the root sectors
		dirIter->dirEntSectorsCap =
				(flopdata->nRootEntries * dirIter->DIR_ENTRY_SIZE) / flopdata->bytesPerSector;
		dirIter->dirEntSectors = malloc(dirIter->dirEntSectorsCap * sizeof(char *));
		int i;
		for (i = 0; i < dirIter->dirEntSectorsCap; i++) {
			dirIter->dirEntSectors[i] = flopdata->rawData +
					(calc_root_start_sector(flopdata) + i) * flopdata->bytesPerSector;
		}
	} else if (is_dir(directory)) {
		dirIter->dirEntSectorsCap = 1;
		dirIter->dirEntSectors = malloc(dirIter->dirEntSectorsCap * sizeof(char *));
		load_directory_sectors(flopdata, directory, &dirIter->dirEntSectors,
				       &dirIter->dirEntSectorsCap);
	} else {
		free(dirIter);
		dirIter = NULL;
	}

	return dirIter;
}


// Destroys the given DirEntryIterator and frees all of its memory
static void destroy_DirEntrYIterator(struct DirEntryIterator *dirIter) {
	free(dirIter->dirEntSectors);
	free(dirIter);
}


// Advances the given DirEntryIterator to the next entry, stores the entry in the given rootent, and returns a
// pointer to the rootent. If there are no more entries, returns NULL.
static struct rootent *DirEntryIterator_next(struct DirEntryIterator *dirIter, struct rootent *ent) {
	char *entData = dirIter->dirEntSectors[dirIter->sectorNum] +
			(dirIter->entryNum * dirIter->DIR_ENTRY_SIZE);

	if (entData[0] == 0) {
		return NULL;
	}

	parse_rootent(entData, ent);

	dirIter->entryNum++;

	// Move to the next sector if we've reached the end of this sector
	if (dirIter->entryNum == dirIter->ENTRIES_PER_SECTOR) {
		dirIter->entryNum = 0;
		dirIter->sectorNum++;
	}

	return ent;
}
