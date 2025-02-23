// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Compressed strings.                                                      │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "m_cps.h"

#define Cps_Ptr(i) (((i) & 3) << 3)

noinit static char buf[4096];

stkoff void Cps_SetC(cps_t *cps, mem_size_t p, i32 c) {
   mem_size_t pp = Cps_Ptr(p);
   cps[p / 4] = cps[p / 4] & ~(0xFF << pp) | byte(c) << pp;
}

stkoff mem_byte_t Cps_GetC(cps_t const *cps, mem_size_t p) {
   return byte(cps[p / 4] >> Cps_Ptr(p));
}

stkoff cstr Cps_Expand(cps_t const *cps, mem_size_t s, mem_size_t l) {
   for(mem_size_t i = 0; i < l; ++i) {
      buf[i] = Cps_GetC(cps, s + i);
   }
   buf[l] = '\0';
   return buf;
}

stkoff cstr Cps_ExpandNT(cps_t const *cps, mem_size_t s) {
   for(mem_size_t i = 0; (buf[i] = Cps_GetC(cps, s + i)); ++i) {
   }
   return buf;
}

/* EOF */
