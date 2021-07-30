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

#define _GNU_SOURCE

#include "common.h"
#include "m_file.h"
#include "w_world.h"

FILE *W_Open(str fname, char rw) {
   char rwStr[3] = {'r', rw == 't' ? '\0' : rw, '\0'};
   i32 lmp = ServCallI(sm_CheckLump, fname);
   if(lmp == -1) {
      return nil;
   } else {
      str f = ServCallS(sm_ReadLump, lmp);
      return __fmemopen_str(f, ACS_StrLen(f), rwStr);
   }
}

FILE *W_OpenIter(str fname, char rw, i32 *prev) {
   char rwStr[3] = {'r', rw == 't' ? '\0' : rw, '\0'};
   i32 lmp = ServCallI(sm_FindLump, fname, *prev);
   if(lmp == -1 || *prev == lmp) {
      return nil;
   } else {
      *prev = lmp;
      str f = ServCallS(sm_ReadLump, lmp);
      return __fmemopen_str(f, ACS_StrLen(f), rwStr);
   }
}

/* EOF */
