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
#include "DirEntryIterator.h"


static void print_file_dump(struct FlopData *, struct rootent *);
static void print_hex_dump_nolabels(const void *, size_t);
static int filename_matches(struct rootent *, char *);
static size_t get_next_filename(char *, char *, off_t);


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
		// Check if filenames match, and if not, go back to the top of the loop
		if (!filename_matches(curEnt, curFilename)) {
			continue;
		}

		// Check if this was the last piece of the path, and if so print the file
		if (path[pathIndex - 1] == '\0' || path[pathIndex] == 0) {
			print_file_dump(flopdata, curEnt);
			foundFile = 1;
			break;
		} else if (is_dir(curEnt)) {
			// Enter the subdirectory
			pathIndex += get_next_filename(curFilename, path, pathIndex) + 1;
			destroy_DirEntryIterator(dirIter);
			dirIter = new_DirEntryIterator(flopdata, curEnt);
			if (dirIter == NULL) {
				fprintf(stderr, "An unexpected error occurred.\n");
			}
			continue;
		} else {
			fprintf(stderr, "Invalid path.\n");
		}
	}
	free(curEnt);
	destroy_DirEntryIterator(dirIter);

	if (!foundFile) {
		fprintf(stderr, "File not found.\n");
	}

	return 0;
}


// Prints the file given by ent as a hex dump
static void print_file_dump(struct FlopData *flopdata, struct rootent *ent) {
	int *sectors = 0;
	int nSectors = get_file_sectors(flopdata, ent, &sectors);

	printf("\n");
	int i;
	for (i = 0; i < nSectors; ++i) {
		off_t offset = flopdata->bytesPerSector * sectors[i];
		print_hex_dump_nolabels((flopdata->rawData + offset), flopdata->bytesPerSector);
	}
	printf("\n");

	free(sectors);
}


// Print a hex dump without any headers or labels
static void print_hex_dump_nolabels(const void *data, size_t datalen) {
	off_t offset = 0;
	while (offset < datalen) {
		int i;
		for (i = 0; i < 16; i++) {
			if (offset < datalen) {
				printf("%5x", ((unsigned char *)data)[offset]);
				offset++;
			} else {
				printf("    ");
			}
		}
		printf("\n");
	}
}


// Checks if the given filename matches (ignoring case) the one found in the given struct rootent
static int filename_matches(struct rootent *ent, char *filename) {
	if (is_deleted(ent) || is_vfat_entry(ent)) {
		return 0;
	}

	char filename_full[13];
	filename_full[0] = '\0';
	strcpy(filename_full, ent->filename);
	if (strcmp(ent->fileext, "") != 0) {
		strcat(filename_full, ".");
		strcat(filename_full, ent->fileext);
	}

	return strcasecmp(filename_full, filename) == 0;
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
