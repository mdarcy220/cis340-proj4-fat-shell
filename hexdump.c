#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hexdump.h"


// Print data as a hex dump
void hexdump(const void *data, size_t datalen) {
	// Check how much padding is needed for left column
	int leftPadding = 1;
	int datalentmp = datalen;
	for(; 0 < datalentmp; datalentmp /= 16) {
		leftPadding++;
	}
	
	
	printf("%*s   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F\n\n", leftPadding, " ");
	
	size_t offset = 0;
	while(offset < datalen) {
		printf("%*X  ", leftPadding, (unsigned int)offset);
		int i;
		for(i = 0; i < 16; i++) {
			if(offset < datalen) {
				char hex[3];
				sprintf(hex, "%2X", ((char*)data)[offset]);
				printf("%s  ", hex);
				offset++;
			}
			else {
				printf("    ");
			}
		}
		printf("\n");
	}
}
