// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_cps.h"

// Extern Functions ----------------------------------------------------------|

char const *Lith_CPS_Print(u32 *cps, int l)
{
   noinit static char buf[4096];
   int i, ch;
   for(i = 0; (ch = Lith_CPS_GetC(cps, i)) && (!l || i < l); i++) buf[i] = ch;
   buf[i] = '\0';
   return buf;
}

// EOF
