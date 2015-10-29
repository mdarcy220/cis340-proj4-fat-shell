#include "flop.h"
#include "fmountutils.h"

int showfile(struct FlopData *flopdata, int argc, char **argv) {
	if(!has_mounted_image(flopdata)) {
		return 1;
	}
	return 0;
}
