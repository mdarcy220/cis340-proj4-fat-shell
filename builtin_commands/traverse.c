
/*
 * Copyright (C) 2017 Mike D'Arcy
 *
 * This file is part of FlopShell, a toy shell utility for floppy disk images.
 * 
 * FlopShell is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * FlopShell is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FlopShell.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "flop.h"
#include "fatroot.h"
#include "fmountutils.h"
#include "DirEntryIterator.h"


static void path_append(char **, char *, size_t *);
static void path_remove_last(char *);
static void print_entry(struct rootent *, char *, int);
static void get_full_filename(struct rootent *, char *);


static const int PRINT_NORMAL = 1;
static const int PRINT_LONGLIST = 2;


// Print all entries in the ROOT table
int command_traverse(struct FlopData *flopdata, int argc, char **argv) {
	if (!has_mounted_image(flopdata)) {
		fprintf(stderr, "Error. Unable to load FAT data. No image mounted.\n");
		return 1;
	}

	int printType = PRINT_NORMAL;
	if (2 <= argc && (strcmp(argv[1], "-l") == 0 || strcmp(argv[1], "l") == 0)) {
		printType = PRINT_LONGLIST;
		printf("        *****************************\n"
		       "        ** FILE ATTRIBUTE NOTATION **\n"
		       "        **                         **\n"
		       "        ** R ------ READ ONLY FILE **\n"
		       "        ** S ------ SYSTEM FILE    **\n"
		       "        ** H ------ HIDDEN FILE    **\n"
		       "        ** A ------ ARCHIVE FILE   **\n"
		       "        *****************************\n\n");
	}

	// Create a stack to hold iterators for each subdirectory
	struct DirEntryIterator **dirIterStack = malloc(sizeof(struct DirEntryIterator *));
	int stackTop = -1;

	// We're traversing the root directory first, so path="/"
	size_t pathLen = 256;
	char *path = malloc(pathLen * sizeof(char));
	path[0] = '/';
	path[1] = '\0';

	// Put an iterator for the root directory onto the stack
	dirIterStack[++stackTop] = new_DirEntryIterator(flopdata, NULL);
	struct rootent *curEnt = malloc(sizeof(*curEnt));

	// Make sure there's at least one entry
	if(!DirEntryIterator_hasNext(dirIterStack[stackTop])) {
		destroy_DirEntryIterator(dirIterStack[stackTop--]);
	}

	// Loop until we reach the end of all subdirectories
	while (0 <= stackTop) {
		// Advance to the next entry
		DirEntryIterator_next(dirIterStack[stackTop], curEnt);
		
		// Print the entry
		if (!is_vfat_entry(curEnt) && !is_deleted(curEnt)) {
			print_entry(curEnt, path, printType);
		}

		// If this is a directory, traverse it
		if (is_dir(curEnt) && strcmp(curEnt->filename, ".") != 0 && strcmp(curEnt->filename, "..") != 0) {
			// Expand the iterator stack and put a new iterator on top of it
			dirIterStack = realloc(dirIterStack, (stackTop + 2) * sizeof(struct DirEntryIterator *));
			dirIterStack[++stackTop] = new_DirEntryIterator(flopdata, curEnt);

			// Add the directory name to the end of the path
			char filename_full[13];
			get_full_filename(curEnt, filename_full);
			path_append(&path, filename_full, &pathLen);
		}

		// If we reached the end of the directory, go back up until there are more entries
		while (0 <= stackTop && !DirEntryIterator_hasNext(dirIterStack[stackTop])) {
			destroy_DirEntryIterator(dirIterStack[stackTop--]);
			path_remove_last(path);
		}
	}

	// Clean up
	free(path);
	free(curEnt);
	free(dirIterStack);

	return 0;
}


// Append the given string to the given path, reallocating the path variable if necessary
static void path_append(char **path, char *filename, size_t *pathLen) {
	size_t totalChars = strlen(*path) + strlen(filename) + 2;
	if (*pathLen < totalChars) {
		*path = realloc(*path, totalChars * sizeof(char));
		*pathLen = totalChars;
	}
	strcat(*path, filename);
	strcat(*path, "/");
}


// Removes the last piece of the given path (e.g., "/path/to/something" becomes "/path/to")
static void path_remove_last(char *path) {
	int i = strlen(path) - 2;
	while (0 <= i && path[i] != '/') {
		i--;
	}
	path[i + 1] = '\0'; // Replace the char after the '/' with the null terminator
}


// Prints the given directory entry located at the given path, in the style specified by the given print type
static void print_entry(struct rootent *ent, char *path, int printType) {
	char filename_full[13];
	get_full_filename(ent, filename_full);
	struct winsize w;
		 ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	int full_length = strlen(path)+strlen(filename_full);


	if (printType == PRINT_NORMAL) {
		char *directoryStr = is_dir(ent) ? "<DIR>" : "     ";
		printf("%s%-*s%s\n", path, (int)(50 - strlen(path)), filename_full, directoryStr);
	} else {
		char attrStr[6] = "-----";
		if (is_archive(ent)) {
			attrStr[1] = 'A';
		}
		if (is_hidden(ent)) {
			attrStr[2] = 'H';
		}
		if (is_system(ent)) {
			attrStr[3] = 'S';
		}
		if (is_readonly(ent)) {
			attrStr[4] = 'R';
		}

		char directoryStr[11];
		if (is_dir(ent)) {
			sprintf(directoryStr, "<DIR>     ");
		} else {
			sprintf(directoryStr, "      %4d", ent->filesize);
		}

		printf("%s     %02d/%02d/%04d %02d:%02d:%02d        %s     %s%s%*s %4d\n", attrStr, ent->date_month,
		       ent->date_day, ent->date_year, ent->time_hour, ent->time_min, ent->time_sec,
		       directoryStr, path, filename_full, w.ws_col-(full_length+60)%w.ws_col, "", ent->first_cluster);
		}
}


// Gets the complete filename for the given directory entry and stores it in the given character pointer
static void get_full_filename(struct rootent *ent, char *buf) {
	buf[0] = '\0';
	strcpy(buf, ent->filename);
	if (strcmp(ent->fileext, "") != 0) {
		strcat(buf, ".");
		strcat(buf, ent->fileext);
	}
}
