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
int is_vfat_entry(struct rootent *);
int parse_rootent(char *, struct rootent *);
int get_file_sectors(struct FlopData *, struct rootent *, int **);


#endif
