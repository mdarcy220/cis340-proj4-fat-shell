
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


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include "shellinput.h"

// Define special key codes
typedef enum KeyCode {
	BACKSPACE = 127,
	SPECIAL_CHAR_CODE = 27,
	ARROW_UP = 65,
	ARROW_DOWN = 66,
	ARROW_RIGHT = 67,
	ARROW_LEFT = 68,
	KEY_DELETE = 51,
	KEY_HOME = 72,
	KEY_END = 70
} KeyCode;


static size_t get_input_line(char **, size_t *);
static void handle_special_char(char **, off_t *, size_t *);


// Gets a line of input for the given FlopShellState and stores it in the given buffer with given capacity
// The capacity will be modified if the buffer needs to be expanded
size_t get_next_input(char **buf, size_t *bufCap) { return get_input_line(buf, bufCap); }


// Gets a line of input
static size_t get_input_line(char **buf, size_t *bufCap) {


	// Save the current terminal settings
	struct termios oldtermio;
	struct termios newtermio;
	tcgetattr(0, &oldtermio);

	// Set the terminal to unbuffered input mode
	newtermio = oldtermio;
	newtermio.c_lflag &= ~ECHO;
	newtermio.c_lflag &= ~ICANON;

	tcsetattr(0, TCSANOW, &newtermio);

	off_t bufPos = 0;
	size_t bufLen = 0;
	(*buf)[bufLen] = '\0';
	char ch;

	while ((ch = getchar())) {
		if (*bufCap < bufLen + 2) {
			*buf = realloc(*buf, (*bufCap) * 2 * sizeof(**buf));
			(*bufCap) *= 2;
		}

		// Backspace
		if ((KeyCode)ch == BACKSPACE && 0 < bufPos) {
			printf("\b");
			int i;
			for (i = bufPos; i < bufLen; i++) {
				printf("%c", (*buf)[i]);
				(*buf)[i-1] = (*buf)[i];
			}
			(*buf)[--bufLen] = '\0';

			// Overwrite the last char
			printf(" ");

			bufPos--;
			for (i = 0; i < (bufLen - bufPos+1); i++) {
				printf("\b");
			}

			continue;
		} else if (ch == 127) {
			continue;
		}

		if (ch == SPECIAL_CHAR_CODE) {
			handle_special_char(buf, &bufPos, &bufLen);
			continue;
		}

		// Print the character
		printf("%c", ch);

		if (ch == '\n') {
			break;
		}

		// Print the rest of the characters (now shifted by one to make space for the new char)
		// Also, shift the characters in the buffer to make space for the new char
		int i;
		for (i = bufPos; i < bufLen; i++) {
			printf("%c", (*buf)[i]);
		}

		for (i = bufLen - 1; bufPos <= i; i--) {
			(*buf)[i + 1] = (*buf)[i];
		}

		(*buf)[bufPos++] = ch;
		bufLen++;
		(*buf)[bufLen] = '\0';

		for (i = 0; i < (bufLen - bufPos); i++) {
			printf("\b");
		}
	}

	// Set the terminal configuration back to the old settings
	tcsetattr(0, TCSANOW, &oldtermio);

	return bufLen;
}


// Handles special characters when getting user input
static void handle_special_char(char **buf, off_t *bufPos, size_t *bufLen) {
	char tmpchar = getchar();

	char ch = getchar();
	KeyCode chCode = (KeyCode)ch;

	switch (chCode) {

	case ARROW_UP:
		break;
	case ARROW_DOWN:
		break;
	case ARROW_RIGHT:
		if (*bufPos < *bufLen) {
			(*bufPos)++;
			printf("%c%c%c", SPECIAL_CHAR_CODE, tmpchar, ch);
		}
		break;
	case ARROW_LEFT:
		if (0 < *bufPos) {
			(*bufPos)--;
			printf("%c%c%c", SPECIAL_CHAR_CODE, tmpchar, ch);
		}
		break;
	case KEY_DELETE:
		getchar(); // Clear the remaining char given for the delete key code
		if (*bufPos < *bufLen) {

			int i;
			for (i = *bufPos + 1; i <= *bufLen; i++) {
				(*buf)[i - 1] = (*buf)[i];
				printf("%c", (*buf)[i]);
			}
			// Overwrite the last char
			printf(" ");

			(*bufLen)--;
			for (i = 0; i < (*bufLen - *bufPos + 1); i++) {
				printf("\b");
			}
		}
		break;
	case KEY_HOME:
		// Decrement bufPos and move cursor back until beginning of input
		for (; 0 < *bufPos; (*bufPos)--) {
			printf("\b");
		}
		break;
	case KEY_END:
		for (; *bufPos < *bufLen; (*bufPos)++) {
			printf("%c", (*buf)[*bufPos]);
		}
		break;
	default:
		break;
	}
}
