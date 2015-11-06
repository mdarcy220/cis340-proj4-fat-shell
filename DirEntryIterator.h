#ifndef DIR_ENTRY_ITERATOR_H_
#define DIR_ENTRY_ITERATOR_H_

#include "flop.h"
#include "fatroot.h"
#include <stdlib.h>

struct DirEntryIterator {
	char **dirEntSectors;
	size_t dirEntSectorsCap;
	int entryNum;
	int sectorNum;

	int DIR_ENTRY_SIZE;
	int ENTRIES_PER_SECTOR;
};

struct DirEntryIterator *new_DirEntryIterator(struct FlopData *, struct rootent *);
void destroy_DirEntryIterator(struct DirEntryIterator *);
struct rootent *DirEntryIterator_next(struct DirEntryIterator *, struct rootent *);

#endif
