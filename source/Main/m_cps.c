// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_cps.h"

// Extern Functions ----------------------------------------------------------|

//
// Lith_CPS_Print
//
__str Lith_CPS_Print(unsigned *cps)
{
   ACS_BeginPrint();

   for(int i = 0, ch; (ch = Lith_CPS_GetC(cps, i)); i++)
      ACS_PrintChar(ch);

   return ACS_EndStrParam();
}

//
// Lith_CPS_Alloc
//
unsigned *Lith_CPS_Alloc(__str s)
{
   int len = ACS_StrLen(s);
   unsigned *ret = calloc(Lith_CPS_Len(len), 1);

   for(int i = 0; i < len; i++)
      Lith_CPS_SetC(ret, i, s[i]);

   return ret;
}

// EOF
