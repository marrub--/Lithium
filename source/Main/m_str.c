// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"

#include <stdio.h>
#include <ctype.h>

#define StrHashImpl() \
   unsigned ret = 0; \
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
unsigned StrHash(char __str_ars const *s)
{
   StrHashImpl();
}

//
// CStrHash
//
unsigned CStrHash(char const *s)
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
__str Lith_ScoreSep(score_t num)
{
   char out[48] = {};

   if(num)
   {
      char *outp = out + 47;
      int cnum = 0;

      while(num)
      {
         *--outp = (num % 10) + '0';
         num = num / 10;

         if(++cnum == 3) {
            *--outp = ',';
            cnum = 0;
         }
      }

      if(!cnum) outp++;

      return StrParam("%s", outp);
   }
   else
      return "0";
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

// EOF
