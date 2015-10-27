#include "flop.h"
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int fd;
int showsector(struct FlopData *flopdata, int argc, char **argv) {
	unsigned short sec;
	int argCount;
	for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
		argCount = 1;
		sec = (unsigned short) atoi(*argv);
	}
	
	unsigned short bytesPerSector;

	if ((fd = open("imagefile.img", O_RDONLY)) < 0) {
		fprintf(stdout, "ERROR mounting floppy\n");
		exit(1);
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
		exit(1);
	}

	bytesRead = read(fd, buf, numOfBytes);
	if (bytesRead != numOfBytes) {
		fprintf(stdout, "Issue reading the floppy.");
		exit(1);
	}

	low = ((unsigned short) buf[11]) & 0xff;
	high = ((unsigned short) buf[12]) & 0xff;
	bytesPerSector = low | (high << 8);

	low = ((unsigned short) buf[22]) & 0xff;
	high = ((unsigned short) buf[23]) & 0xff;

	offset = lseek(fd, bytesPerSector, 0);
	if (offset != bytesPerSector) {
		fprintf(stdout, "Issue setting the cursor");
		exit(1);
	}

	int i;
	unsigned char hex[bytesPerSector];
	fprintf(stdout, "\nhex dump of sector : %d", sec);

	// set up horizontal hex values
	fprintf(stdout, "\n");
	fprintf(stdout, "\t 0 \t 1 \t 2 \t 3 \t 4 \t 5 \t 6 \t 7 \t 8 \t 9 \t A \t B \t C \t D \t E \t F");

	lseek(fd,sec*bytesPerSector,SEEK_SET);
	for(i=0;i<bytesPerSector;i++) {
		read(fd,&hex[i],1);
		if(i % 16 == 0){
			fprintf(stdout, "\n %x ",i);
		}
		fprintf(stdout, "\t %x ", hex[i]);
	}
	fprintf(stdout, "\n");
	close(fd);
	return 0;
}
