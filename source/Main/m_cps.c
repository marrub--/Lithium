// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_cps.h"

// Extern Functions ----------------------------------------------------------|

__str Lith_CPS_Print(u32 *cps)
{
   ACS_BeginPrint();

   for(int i = 0, ch; (ch = Lith_CPS_GetC(cps, i)); i++)
      ACS_PrintChar(ch);

   return ACS_EndStrParam();
}

// EOF
