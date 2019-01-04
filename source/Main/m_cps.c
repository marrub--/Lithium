// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#include "common.h"
#include "m_cps.h"

// Extern Functions ----------------------------------------------------------|

char const *Cps_Print(u32 *cps, i32 l)
{
   noinit static char buf[4096];
   i32 i, ch;
   for(i = 0; (ch = Cps_GetC(cps, i)) && (!l || i < l); i++) buf[i] = ch;
   buf[i] = '\0';
   return buf;
}

// EOF
