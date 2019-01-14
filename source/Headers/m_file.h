// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// m_file.h: I/O stream handling.

#ifndef m_file_h
#define m_file_h

#include <stdio.h>

#define W_Find(name) ServCallI(s"FindLump", name)
#define W_Read(lmp)  ServCallS(s"ReadLump", lmp)

#define FEOF(fp) ((fp)->_flag & _FILEFLAG_EOF)

FILE *W_Open(str fname, char const *rw);
FILE *NFOpen(i32 pnum, str pcvar, char rw);
size_t FWrite32(void const *restrict ptr, size_t count, size_t bytes, FILE *restrict fp);
size_t FWrite(void const *restrict ptr, size_t count, FILE *restrict fp);
size_t FWriteStr(void const __str_ars *restrict ptr, size_t count, FILE *restrict fp);
size_t FRead32(void *restrict buf, size_t count, size_t bytes, FILE *restrict fp);

#endif
