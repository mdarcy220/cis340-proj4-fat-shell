#ifndef STRUCTURE_H_
#define STRUCTURE_H_

#include "flop.h"

int print_fs_structure(struct FlopData *, int, char **);
int get_fs_structure(struct FlopData*);
int load_fs_structure(struct FlopData*);
int calc_root_start_sector(struct FlopData*);

#endif
