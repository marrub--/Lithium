// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#include "common.h"

#include <stdio.h>

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
   i32 pos = 0;

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
