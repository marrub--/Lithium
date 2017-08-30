#ifndef LITH_FILE_H
#define LITH_FILE_H

#include <stdio.h>

FILE *Lith_NFOpen(int pnum, __str pcvar, char rw);
size_t Lith_FWrite32(void const *ptr, size_t count, size_t bytes, FILE *fp);
size_t Lith_FRead32(void *buf, size_t count, size_t bytes, FILE *fp);

#endif

