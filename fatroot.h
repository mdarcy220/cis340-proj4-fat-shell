#ifndef FATROOT_H_
#define FATROOT_H_


struct rootent {
	char *filename;
	char *fileext;
	char attribute;
	
	int first_cluster;
	int filesize;
	
};


int is_dir(struct rootent*);
int parse_rootent(char*, struct rootent*);




#endif
