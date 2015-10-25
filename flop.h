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
	int sectorSize;
};

#endif
