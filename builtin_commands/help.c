#include <stdio.h>
#include "help.h"


// The help shell command
int command_help(struct FlopData *flopdata, int argc, char **argv) {
	print_help();
	return 0;
}


// Print help
void print_help() {
	
        printf("\nThe shell has the following commands.\n\n");
        printf("usage___________________________|result_______________________\n");
        printf("cd [directory] \t\t\t|change current directory.\n");
        printf("path [+/-] [pathname] \t\t|shows working path or appends current search path\n");             
        printf("showfat\t\t\t\t|shows the fat1 table for mounted image.\n");
        printf("traverse\t\t\t|shows directory entries on image.\n");
        printf("showfile [directory entry]\t|shows file raw data.\n");
	printf("structure\t\t\t|shows structure of disk.\n");
	printf("showsector [sector #]\t\t|show specified sector #.\n");
        printf("quit or q\t\t\t|texit out of the shell.\n\n");

}
