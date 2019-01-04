// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#include "common.h"

#include <stdio.h>
#include <ctype.h>

#define CpyStrLocal(out, st) \
   do { \
      ACS_BeginPrint(); \
      ACS_PrintLocalized(st); \
      str s = ACS_EndStrParam(); \
      for(i32 i = 0, l = ACS_StrLen(s); i <= l; i++) out[i] = s[i]; \
   } while(0)

#define StrHashImpl() \
   u32 ret = 0; \
   for(; *s; s++) ret = *s + 101 * ret; \
   return ret

StrEntON
#define X(n, s) str const n = s;
#include "m_stab.h"
StrEntOFF

str l_strupper(str in)
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

i32 lstrcmp_str(char const *s1, char __str_ars const *s2)
{
   StrCmpImpl();
}

stkcall
str scoresep(i96 num)
{
   static char out[48];

   if(!num) return st_0;

   char *outp = out + countof(out) - 1;
   i32 cnum = 0;

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

   return l_strdup(outp);
}

str LanguageV(str name)
{
   ACS_BeginPrint();
   ACS_PrintLocalized(name);
   str ret = ACS_EndStrParam();

   while(ret[0] == '$')
   {
      str sub = ACS_StrMid(ret, 1, 0x7FFFFFFF);
      ACS_BeginPrint();
      ACS_PrintLocalized(sub);
      str nex = ACS_EndStrParam();
      if(sub != nex) ret = nex;
      else           break;
   }

   return ret;
}

char *LanguageVC(char *out, char const *name)
{
   noinit static char sbuf[8192];

   if(!out) out = sbuf;
   CpyStrLocal(out, l_strdup(name));

   while(out[0] == '$') CpyStrLocal(out, l_strdup(&out[1]));

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

str LanguageNull(char const *fmt, ...)
{
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   str name = ACS_EndStrParam();
   str alias = LanguageV(name);

   return name == alias ? nil : alias;
}

// EOF
