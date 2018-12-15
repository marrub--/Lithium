// Copyright © 2016-2018 Alison Sanderson, all rights reserved.
#include "lith_common.h"

#include <stdio.h>
#include <ctype.h>

StrEntOFF

#define StrHashImpl() \
   u32 ret = 0; \
   for(; *s; s++) ret = *s + 101 * ret; \
   return ret

__str l_strupper(__str in)
{
   ACS_BeginPrint();

   for(char __str_ars const *c = in; *c; c++)
      ACS_PrintChar(toupper(*c));

   return ACS_EndStrParam();
}

stkcall
u32 l_strhash(char __str_ars const *s)
{
   StrHashImpl();
}

stkcall
u32 lstrhash(char const *s)
{
   StrHashImpl();
}

char *lstrcpy_str(char *dest, char __str_ars const *src)
{
   for(char *i = dest; (*i = *src); ++i, ++src);
   return dest;
}

#define StrCmpImpl() \
   for(; *s1 && *s2; ++s1, ++s2) {if(*s1 != *s2) return *s1 - *s2;} \
   return *s1 - *s2

int lstrcmp_str(char const *s1, char __str_ars const *s2)
{
   StrCmpImpl();
}

stkcall
__str scoresep(i96 num)
{
   static char out[48];

   if(!num) return s"0";

   char *outp = out + countof(out) - 1;
   int cnum = 0;

   while(num)
   {
      lldiv_t div = lldiv(num, 10);
      *--outp = div.rem + '0';
      num = div.quot;

      if(++cnum == 3) {
         *--outp = ',';
         cnum = 0;
      }
   }

   if(!cnum) outp++;

   ACS_BeginPrint();
   ACS_PrintGlobalCharArray((int)outp, __GDCC__Sta);
   return ACS_EndStrParam();
}

__str LanguageV(__str name)
{
   ACS_BeginPrint();
   ACS_PrintLocalized(name);
   __str ret = ACS_EndStrParam();

   while(ret[0] == '$')
   {
      __str sub = ACS_StrMid(ret, 1, 0x7FFFFFFF);
      ACS_BeginPrint();
      ACS_PrintLocalized(sub);
      __str nex = ACS_EndStrParam();
      if(sub != nex) ret = nex;
      else           break;
   }

   return ret;
}

char *LanguageVC(char *out, char const *name)
{
   noinit static char sbuf[8192];
   if(!out) out = sbuf;

   sprintf(out, "%LS", l_strdup(name));

   while(out[0] == '$') sprintf(out, "%LS", l_strdup(&out[1]));

   return out;
}

char *LanguageCV(char *out, char const *fmt, ...)
{
   noinit static char nbuf[256];
   va_list vl;

   va_start(vl, fmt);
   vsprintf(nbuf, fmt, vl);
   va_end(vl);

   return LanguageVC(out, nbuf);
}

__str LanguageNull(__str fmt, ...)
{
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);

   __str name = ACS_EndStrParam();
   __str alias = LanguageV(name);

   return name == alias ? null : alias;
}

// EOF
