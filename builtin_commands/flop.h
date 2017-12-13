
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

#ifndef FLOP_H_
#define FLOP_H_

#include <stdlib.h>

// Struct that holds disk image data
struct FlopData {
	// Raw image data
	char * rawData;
	
	// Length, in bytes, of the raw image data
	size_t rawDataLen;
	
	// FAT entry width (FAT12, FAT16, FAT32)
	int fatEntryWidth;
	
	// Number of FAT tables in the filesystem
	int nFatTables;
	
	// Size in sectors of each FAT table
	int sectorsPerFat;
	
	// Number of sectors in each cluster
	int sectorsPerCluster;
	
	// Number of entries in ROOT
	int nRootEntries;
	
	// Size of each sector, in bytes
	int bytesPerSector;
	
	// Number of reserved sectors preceeding FAT tables
	int nReservedSectors;
};

#endif
