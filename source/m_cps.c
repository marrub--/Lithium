// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Compressed strings.                                                      │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "m_cps.h"

#define Cps_Shift(i, set) ((set) << ((i) % 4 * 8))

noinit static
char buf[4096];

alloc_aut(0) stkcall
void Cps_SetC(cps_t *cps, mem_size_t p, char c) {
   cps[p / 4] &= ~Cps_Shift(p,     0xFF);
   cps[p / 4] |=  Cps_Shift(p, c & 0xFF);
}

alloc_aut(0) stkcall
mem_byte_t Cps_GetC(cps_t const *cps, mem_size_t p) {
   return byte((cps[p / 4] & (0xFF << (p % 4 * 8))) >> (p % 4 * 8));
}

alloc_aut(0) stkcall
cstr Cps_Expand(cps_t const *cps, mem_size_t s, mem_size_t l) {
   mem_size_t i;
   for(i = 0; i < l;) buf[i++] = Cps_GetC(cps, i + s);
   buf[i] = '\0';
   return buf;
}

alloc_aut(0) stkcall
cstr Cps_ExpandNT(cps_t const *cps, mem_size_t s) {
   mem_size_t i;
   char ch;
   for(i = 0; (ch = Cps_GetC(cps, i + s));) buf[i++] = ch;
   buf[i] = '\0';
   return buf;
}

/* EOF */
