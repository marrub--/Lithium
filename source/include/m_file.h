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

FILE *W_Open(str fname, cstr rw);
FILE *NFOpen(str pcvar, char rw);
size_t FWrite32(void const *restrict ptr, size_t count, size_t bytes, FILE *restrict fp);
size_t FWriteStr(void const __str_ars *restrict ptr, size_t count, FILE *restrict fp);
size_t FRead32(void *restrict buf, size_t count, size_t bytes, FILE *restrict fp);

#endif
