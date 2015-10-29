#include <string.h>

#include "flop.h"
#include "fatroot.h"
#include "concatbytes.h"

// Checks if the given root entry is a directory
int is_dir(struct rootent *entry) {
	return (entry->attribute & 0x4);
}


// Parses the given data (should be 32 bytes) into a root entry
int parse_rootent(char *data, struct rootent *entry) {
	strncpy(entry->filename, data, 8);
	strncpy(entry->fileext, data+8, 3);
	entry->attribute = data[11];
	
	entry->first_cluster = concat_uint8_uint16(data[27], data[26]);
	
	entry->filesize = concat_uint8_uint32(data[31], data[30], data[29], data[28]);
	
	return 0;
}

