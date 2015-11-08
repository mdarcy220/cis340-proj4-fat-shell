#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hexdump.h"

static int calc_left_padding(size_t);

// Print data as a hex dump
void print_hex_dump(const void *data, size_t datalen) {
	// Check how much padding is needed for left column
	int leftPadding = calc_left_padding(datalen);

	printf("%*s   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F\n\n", leftPadding, " ");
	append_hex_dump(data, datalen, 0);
}


// Print a hex dump of the given data, but start numbering rows at the given number and don't print a header
void append_hex_dump(const void *data, size_t datalen, off_t firstrownum) {
	// Check how much padding is needed for left column
	int leftPadding = calc_left_padding(datalen);

	off_t offset = 0;
	while (offset < datalen) {
		printf("%*X  ", leftPadding, (unsigned int)(offset + firstrownum));
		int i;
		for (i = 0; i < 16; i++) {
			if (offset < datalen) {
				printf("%2X  ", ((unsigned char *)data)[offset]);
				offset++;
			} else {
				printf("    ");
			}
		}
		printf("\n");
	}
}


// Calculates padding for data of given length
static int calc_left_padding(size_t datalen) {
	int leftPadding = 1;
	size_t datalentmp = datalen;
	for (; 0 < datalentmp; datalentmp /= 16) {
		leftPadding++;
	}

	return leftPadding;
}
