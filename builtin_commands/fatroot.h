
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

#ifndef FATROOT_H_
#define FATROOT_H_


struct rootent {
	char filename[9];
	char fileext[4];
	char attribute;

	int first_cluster;
	int filesize;

	unsigned short time_hour;
	unsigned short time_min;
	unsigned short time_sec;

	unsigned short date_year;
	unsigned short date_month;
	unsigned short date_day;
};


int is_dir(struct rootent *);
int is_archive(struct rootent*);
int is_hidden(struct rootent*);
int is_system(struct rootent*);
int is_readonly(struct rootent*);

int is_vfat_entry(struct rootent *);
int is_deleted(struct rootent*);

int parse_rootent(char *, struct rootent *);
int get_file_sectors(struct FlopData *, struct rootent *, int **);


#endif

