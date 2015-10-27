#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "showfat.h"
#include "flop.h"
#include "fmountutils.h"
#include "structure.h"

int fd;

int showfat(struct FlopData *flopdata, int argc, char **argv) {
	 
	unsigned short bytesPerSector;
	unsigned short numOfSectors;
	unsigned short fatBytes;
	unsigned short cluster;
	char *fatBuffer;
	
	if(!has_mounted_image(flopdata)) {
		fprintf(stderr, "Error. No image mounted.\n");
		return 1;
	}
	
	if(load_fs_structure(flopdata) != 0) {
		fprintf(stderr, "Error. Filesystem structure data could not be loaded.\n");
		return 1;
	}

	if ((fd = open("imagefile.img", O_RDONLY)) < 0) {
		fprintf(stdout, "ERROR mounting floppy\n");
		return 1;
	}

	unsigned short low, high;
	char buf[32];
	size_t numOfBytes;
	ssize_t bytesRead;
	numOfBytes = sizeof(buf);
	off_t offset;

	offset = lseek(fd, SEEK_SET, SEEK_SET);
	if (offset == -1) {
		fprintf(stdout, "Issue reading beginning of floppy.");
		return 1;
	}

	bytesRead = read(fd, buf, numOfBytes);
	if (bytesRead != numOfBytes) {
		fprintf(stdout, "Issue reading the floppy.");
		return 1;
	}


	low = ((unsigned short) buf[22]) & 0xff;
	high = ((unsigned short) buf[23]) & 0xff;
	numOfSectors = low | (high << 8);

	low = ((unsigned short) buf[11]) & 0xff;
	high = ((unsigned short) buf[12]) & 0xff;
	bytesPerSector = low | (high << 8);


	fatBytes = bytesPerSector * numOfSectors;

	fatBuffer = (char *) malloc(fatBytes);
	offset = lseek(fd, bytesPerSector, 0);
	if (offset != bytesPerSector) {
		fprintf(stdout, "Issue setting the cursor");
		return 1;
	}

	if ((bytesRead = read(fd, fatBuffer, fatBytes)) != fatBytes) {
		fprintf(stdout, "Issue reading the sector");
		return 1;
	}

	// set up horizontal hex values
	fprintf(stdout, "\n");
	for (int i=0;i<16;i++) {
		fprintf(stdout, "\t %x", i);
	}

	fprintf(stdout, "\n \t \t"); // first two entries are reserved
	for (int x = 2; x < fatBytes*2/3; x++) {
		unsigned short low, high;
		unsigned short temp = (unsigned short) x;
		// prints the vertical hex values
		if ((x%16) == 0) {
			fprintf(stdout, "\n %x", x);
		}

		if (temp%2) {
			low = (((unsigned short) fatBuffer[(3*temp - 1)/2])>>4) & 0x000f;
			high = (((unsigned short) fatBuffer[(3*temp + 1)/2])<<4) & 0x0ff0;
		}else {
			low = ((unsigned short) fatBuffer[3*temp/2]) & 0x00ff;
			high = (((unsigned short) fatBuffer[(3*temp + 2)/2])<<8) & 0x0f00;
		}
		cluster = low | high;

		if (cluster)
			fprintf(stdout, "\t %x", cluster);
		else
			fprintf(stdout, "\t FREE");
	}

	fprintf(stdout, "\n");
	free(fatBuffer);
	close(fd);
	return 0;
}
