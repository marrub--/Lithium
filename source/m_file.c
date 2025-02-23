// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ I/O stream handling.                                                     │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#define _GNU_SOURCE

#include "m_engine.h"
#include "m_file.h"
#include "w_world.h"

FILE *W_OpenNum(i32 lump, char rw) {
   static char rwStr[3] = "r";
   rwStr[1] = rw == 't' ? '\0' : rw;
   if(lump == -1) {
      return nil;
   } else {
      str f = ServCallS(sm_ReadLump, lump, rw == 't');
      return __fmemopen_str(f, ACS_StrLen(f), rwStr);
   }
}

FILE *W_Open(str fname, char rw) {
   return W_OpenNum(ServCallI(sm_CheckLump, fname), rw);
}

FILE *W_OpenIter(str fname, char rw, i32 *prev) {
   char rwStr[3] = {'r', rw == 't' ? '\0' : rw, '\0'};
   i32 lmp = ServCallI(sm_FindLump, fname, *prev);
   if(lmp == -1) {
      return nil;
   } else {
      *prev = (lmp == -1 &? -1) |? lmp + 1;
      str f = ServCallS(sm_ReadLump, lmp, rw == 't');
      return __fmemopen_str(f, ACS_StrLen(f), rwStr);
   }
}

/* EOF */
