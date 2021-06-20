/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Compressed strings.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "m_cps.h"

#define Cps_Shift(i, set) ((set) << ((i) % 4 * 8))

noinit static
char buf[4096];

void Cps_SetC(u32 *cps, u32 p, u32 c) {
   cps[p / 4] &= ~Cps_Shift(p, 0xFF);
   cps[p / 4] |=  Cps_Shift(p, c & 0xFF);
}

byte Cps_GetC(u32 const *cps, u32 p) {
   return (cps[p / 4] & (0xFF << (p % 4 * 8))) >> (p % 4 * 8);
}

cstr Cps_Expand(u32 *cps, u32 s, u32 l) {
   u32 i;
   for(i = 0; i < l;) buf[i++] = Cps_GetC(cps, i + s);
   buf[i] = '\0';
   return buf;
}

cstr Cps_ExpandNT(u32 *cps, u32 s) {
   u32 i;
   char ch;
   for(i = 0; (ch = Cps_GetC(cps, i + s));) buf[i++] = ch;
   buf[i] = '\0';
   return buf;
}

/* EOF */
