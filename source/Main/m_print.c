// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"

#include <stdio.h>

StrEntOFF

void Log(char const *fmt, ...)
{
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   ACS_EndLog();
}

script
void Lith_PrintMem(void const *data, size_t size)
{
   byte const *d = data;
   int pos = 0;

   for(size_t i = 0; i < size; i++)
   {
      if(pos + 3 > 79)
      {
         puts("");
         pos = 0;
      }

      printf(IsPrint(d[i]) ? "%c  " : c"%.2X ",  d[i]);

      pos += 3;
   }

   puts("\nEOF\n\n");
}

// EOF
