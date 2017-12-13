
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



// Concatenates two bytes into an unsigned short (uint16)
unsigned short concat_uint8_uint16(unsigned char b1, unsigned char b2) {
	return (unsigned short)(((unsigned short) b1) << 8) | ((unsigned short) b2);
}


// Concatenates four bytes into an unsigned int (uint32)
unsigned int concat_uint8_uint32(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4) {
	return (unsigned int)((((unsigned int) b1) << 24) | (((unsigned int) b2) << 16) | (((unsigned int) b3) << 8) | ((unsigned int) b4));
}
