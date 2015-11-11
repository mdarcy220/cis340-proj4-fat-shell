#ifndef FMOUNTUTILS_H_
#define FMOUNTUTILS_H_

#include "flop.h"

int command_fmount(struct FlopData*, int, char**);
int command_fumount(struct FlopData*, int, char**);
int fumount(struct FlopData*);
int fmount(struct FlopData*, char*);
int has_mounted_image(struct FlopData*);

#endif
