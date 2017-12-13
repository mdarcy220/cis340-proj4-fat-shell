
/*
 * Copyright (C) 2017 Mike D'Arcy
 *
 * This file is part of FlopShell, a toy shell utility for floppy disk images.
 * 
 * FlopShell is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * FlopShell is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FlopShell.  If not, see <http://www.gnu.org/licenses/>.
 */

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
int DirEntryIterator_hasNext(struct DirEntryIterator *);

#endif

