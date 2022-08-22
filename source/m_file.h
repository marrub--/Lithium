// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ I/O stream handling.                                                     │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef m_file_h
#define m_file_h

#include <stdio.h>

#define FEOF(fp) ((fp)->_flag & _FILEFLAG_EOF)
FILE *W_OpenNum(i32 lump, char rw);
FILE *W_Open(str fname, char rw);
FILE *W_OpenIter(str fname, char rw, i32 *prev);

#endif
