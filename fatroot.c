#include <stdlib.h>
#include <string.h>

#include "flop.h"
#include "fatroot.h"
#include "concatbytes.h"

static int trim_trailing_spaces(char*);


// Checks if the given root entry is a directory
int is_dir(struct rootent *entry) {
	return (entry->attribute & 0x4);
}


// Parses the given data (should be 32 bytes) into a root entry
int parse_rootent(char *data, struct rootent *entry) {
	strncpy(entry->filename, data, 8);
	entry->filename[8] = '\0';
	trim_trailing_spaces(entry->filename);
	
	strncpy(entry->fileext, data+8, 3);
	entry->fileext[3] = '\0';
	trim_trailing_spaces(entry->fileext);
	
	entry->attribute = data[11];
	
	entry->first_cluster = concat_uint8_uint16(data[27], data[26]);
	
	entry->filesize = concat_uint8_uint32(data[31], data[30], data[29], data[28]);
	
	unsigned short time_tmp = concat_uint8_uint16(data[23], data[22]);
	entry->time_hour = time_tmp >> 11; // First 5 bits
	entry->time_min = (time_tmp & 0x07E0) >> 5; // Middle 6 bits
	entry->time_sec = time_tmp & 0x001F; // Last 5 bits
	
	unsigned short date_tmp = concat_uint8_uint16(data[25], data[24]);
	entry->date_year = date_tmp >> 9; // First 7 bits
	entry->date_month = (date_tmp & 0x01E0) >> 5; // Middle 4 bits
	entry->date_day = date_tmp & 0x001F; // Last 5 bits
	
	return 0;
}


// Removes trailing spaces from the given string
static int trim_trailing_spaces(char *str) {
	int i = strlen(str)-1;
	while (0 <= i && (str[i] == ' ' || str[i] == '\0')) {
		i--;
	}
	
	str[i+1] = '\0';
	
	return 0;
}
