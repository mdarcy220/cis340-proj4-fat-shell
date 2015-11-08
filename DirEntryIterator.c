#include "DirEntryIterator.h"
#include "structure.h"

static void load_directory_sectors(struct FlopData *, struct rootent *, char ***, size_t *);

/*
 * Creates a new DirEntryIterator and returns it (it and many of its members are malloc()'ed, and it should
 * only be deconstructed using destroy_DirEntrYIterator; don't free() it manually). The iterator will be set
 * to iterate over the given directory, unless the given directory is NULL, in which case it will iteratoe
 * over the ROOT directory. Returns NULL if the given rootent isn't a subdirectory.
 */
struct DirEntryIterator *new_DirEntryIterator(struct FlopData *flopdata, struct rootent *directory) {
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
void destroy_DirEntryIterator(struct DirEntryIterator *dirIter) {
	free(dirIter->dirEntSectors);
	free(dirIter);
}


// Advances the given DirEntryIterator to the next entry, stores the entry in the given rootent, and returns a
// pointer to the rootent. If there are no more entries, returns NULL.
struct rootent *DirEntryIterator_next(struct DirEntryIterator *dirIter, struct rootent *ent) {
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


// Checks if there are entries left in the directory
int DirEntryIterator_hasNext(struct DirEntryIterator *dirIter) {
	char *entData = dirIter->dirEntSectors[dirIter->sectorNum] +
			(dirIter->entryNum * dirIter->DIR_ENTRY_SIZE);

	return (entData[0] != 0);
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

