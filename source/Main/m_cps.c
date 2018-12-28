// Copyright © 2018 Alison Sanderson, all rights reserved.
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
