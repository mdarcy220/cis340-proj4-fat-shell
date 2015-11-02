#ifndef HEXDUMP_H_
#define HEXDUMP_H_

#include <stdlib.h>

void print_hex_dump(const void*, size_t);
void append_hex_dump(const void*, size_t, off_t);

#endif
