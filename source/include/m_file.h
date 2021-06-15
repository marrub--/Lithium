/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * I/O stream handling.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef m_file_h
#define m_file_h

#include <stdio.h>

#define FEOF(fp) ((fp)->_flag & _FILEFLAG_EOF)

FILE *W_Open(str fname, char rw);
FILE *W_OpenIter(str fname, char rw, i32 *prev);
FILE *NFOpen(str pcvar, char rw);
mem_size_t FWrite32(void const *restrict ptr, mem_size_t count, mem_size_t bytes, FILE *restrict fp);
mem_size_t FWriteStr(void const __str_ars *restrict ptr, mem_size_t count, FILE *restrict fp);
mem_size_t FRead32(void *restrict buf, mem_size_t count, mem_size_t bytes, FILE *restrict fp);

#endif
