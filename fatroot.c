#include <stdlib.h>
#include <string.h>

#include "flop.h"
#include "fatroot.h"
#include "structure.h"
#include "concatbytes.h"

static int trim_trailing_spaces(char *);
static int cluster2sector(struct FlopData *, int);


// Checks if the given root entry is a directory
int is_dir(struct rootent *entry) {
	return (entry->attribute & 0x10); // stored in bit 4
}


// Checks if the given root entry is an archive file
int is_archive(struct rootent *entry) {
	return (entry->attribute & 0x20); // stored in bit 5
}


// Checks if the given root entry is hidden
int is_hidden(struct rootent *entry) {
	return (entry->attribute & 0x02); // stored in bit 1
}


// Checks if the given root entry is a system file
int is_system(struct rootent *entry) {
	return (entry->attribute & 0x04); // stored in bit 2
}


// Checks if the given root entry is read-only
int is_readonly(struct rootent *entry) {
	return (entry->attribute & 0x01); // stored in bit 0
}


// Checks if the given directory entry is a special VFAT entry (marked by attribute byte 0xf)
int is_vfat_entry(struct rootent *entry) { 
	return (entry->attribute == 0x0F); 
}


// Checks if the given directory entry is for a deleted file
int is_deleted(struct rootent *entry) { 
	return (unsigned char)(entry->filename[0]) == (unsigned char)0xE5; 
}


// Parses the given data (should be 32 bytes) into a root entry
int parse_rootent(char *data, struct rootent *entry) {
	strncpy(entry->filename, data, 8);
	entry->filename[8] = '\0';
	trim_trailing_spaces(entry->filename);

	strncpy(entry->fileext, data + 8, 3);
	entry->fileext[3] = '\0';
	trim_trailing_spaces(entry->fileext);

	entry->attribute = data[11];

	entry->first_cluster = concat_uint8_uint16(data[27], data[26]);

	entry->filesize = concat_uint8_uint32(data[31], data[30], data[29], data[28]);

	unsigned short time_tmp = concat_uint8_uint16(data[23], data[22]);
	entry->time_hour = time_tmp >> 11;	  // First 5 bits
	entry->time_min = (time_tmp & 0x07E0) >> 5; // Middle 6 bits
	entry->time_sec = time_tmp & 0x001F;	// Last 5 bits

	unsigned short date_tmp = concat_uint8_uint16(data[25], data[24]);
	entry->date_year = (date_tmp >> 9) + 1980;    // First 7 bits
	entry->date_month = (date_tmp & 0x01E0) >> 5; // Middle 4 bits
	entry->date_day = date_tmp & 0x001F;	  // Last 5 bits

	return 0;
}


// Removes trailing spaces from the given string
static int trim_trailing_spaces(char *str) {
	int i = strlen(str) - 1;
	while (0 <= i && (str[i] == ' ' || str[i] == '\0')) {
		i--;
	}

	str[i + 1] = '\0';

	return 0;
}


// Prints a file starting at the given cluster as a hex dump
int get_file_sectors(struct FlopData *flopdata, struct rootent *fileEnt, int **sectors) {
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


// Converts the given cluster number to the corresponding sector number, and returns the sector number
static int cluster2sector(struct FlopData *flopdata, int clusterNum) {
	return clusterNum * flopdata->sectorsPerCluster + calc_data_start_sector(flopdata) - 2;
}

