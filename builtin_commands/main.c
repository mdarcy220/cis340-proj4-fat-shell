#include "traverse.h"
#include "structure.h"
#include "showsector.h"
#include "showfile.h"
#include "showfat.h"
#include "help.h"
#include "fmountutils.h"

#include "flop.h"
#include <stdlib.h>


#ifdef HELP_MAIN

int main(int argc, char **argv) {
	return command_help(NULL, argc, argv);
}

#endif


#ifdef SHOWFAT_MAIN

int main(int argc, char **argv) {
	struct FlopData *data = calloc(1, sizeof(struct FlopData));
	fmount(data, "../test/imagefile.img");
	return command_showfat(data, argc, argv);
}

#endif


#ifdef SHOWFILE_MAIN

int main(int argc, char **argv) {
	struct FlopData *data = calloc(1, sizeof(struct FlopData));
	fmount(data, "../test/imagefile.img");
	return command_showfile(data, argc, argv);
}

#endif


#ifdef SHOWSECTOR_MAIN

int main(int argc, char **argv) {
	struct FlopData *data = calloc(1, sizeof(struct FlopData));
	fmount(data, "../test/imagefile.img");
	return command_showsector(data, argc, argv);
}

#endif


#ifdef STRUCTURE_MAIN

int main(int argc, char **argv) {
	struct FlopData *data = calloc(1, sizeof(struct FlopData));
	fmount(data, "../test/imagefile.img");
	return print_fs_structure(data, argc, argv);
}

#endif


#ifdef TRAVERSE_MAIN

int main(int argc, char **argv) {
	struct FlopData *data = calloc(1, sizeof(struct FlopData));
	fmount(data, "../test/imagefile.img");
	return command_traverse(data, argc, argv);
}

#endif



