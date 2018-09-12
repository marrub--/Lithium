// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"

#include <stdio.h>
#include <ctype.h>
#include <GDCC.h>

#define StrHashImpl() \
   u32 ret = 0; \
   for(; *s; s++) ret = *s + 101 * ret; \
   return ret

//
// StrUpper
//
__str StrUpper(__str in)
{
   ACS_BeginStrParam();

   for(char __str_ars const *c = in; *c; c++)
      ACS_PrintChar(toupper(*c));

   return ACS_EndStrParam();
}

//
// StrHash
//
stkcall
u32 StrHash(char __str_ars const *s)
{
   StrHashImpl();
}

//
// CStrHash
//
stkcall
u32 CStrHash(char const *s)
{
   StrHashImpl();
}

//
// Lith_strcpy_str
//
char *Lith_strcpy_str(char *dest, char __str_ars const *src)
{
   for(char *i = dest; (*i = *src); ++i, ++src);
   return dest;
}

#define StrCmpImpl() \
   for(; *s1 && *s2; ++s1, ++s2) {if(*s1 != *s2) return *s1 - *s2;} \
   return *s1 - *s2

//
// Lith_strcmp_str
//
int Lith_strcmp_str(char const *s1, char __str_ars const *s2)
{
   StrCmpImpl();
}

//
// Lith_ScoreSep
//
stkcall
__str Lith_ScoreSep(i96 num)
{
   static char out[48];

   if(!num) return "0";

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

//
// LanguageV
//
static __str LanguageV(__str name)
{
   __str ret = StrParam("%LS", name);

   if(ret[0] == '$')
   {
      __str sub = ACS_StrMid(ret, 1, 0x7FFFFFFF);
      __str nex = StrParam("%LS", sub);
      if(sub != nex)
         ret = nex;
   }

   return ret;
}

//
// Language
//
__str Language(__str fmt, ...)
{
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);

   return LanguageV(ACS_EndStrParam());
}

//
// LanguageNull
//
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

//
// StrParam
//
__str StrParam(__str fmt, ...)
{
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);

   return ACS_EndStrParam();
}

//
// StrParamBegin
//
void StrParamBegin(__str fmt, ...)
{
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);
}

// EOF
