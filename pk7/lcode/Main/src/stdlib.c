//-----------------------------------------------------------------------------
//
// Copyright © 2016-2017 Project Golan
//
// See "LICENSE.lithos3" for more information.
//
//-----------------------------------------------------------------------------
//
// Miscallaneous library functions.
//
//-----------------------------------------------------------------------------

#include "Lth.h"

#include <GDCC.h>

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdfix.h>
#include <ACS_ZDoom.h>


// Extern Functions ----------------------------------------------------------|

//
// Lth_fpeekc
//
int Lth_fpeekc(FILE *fp)
{
   int ret = fgetc(fp);
   ungetc(ret, fp);
   return ret;
}

//
// Lth_strdup
//
// Duplicates a string, allocating a new one.
//
char *Lth_strdup(char const *s)
{
   size_t len = strlen(s);
   char  *ret = malloc(len + 1);
   memmove(ret, s, len);
   ret[len] = '\0';
   return ret;
}

//
// Lth_strdup_str
//
// Duplicates a string entity, allocating a new string.
//
char *Lth_strdup_str(__str s)
{
   size_t len = ACS_StrLen(s);
   char  *ret = malloc(len + 1);
   ACS_StrArsCpyToGlobalCharRange((int)ret, __GDCC__Sta, 0, len, s);
   ret[len] = '\0';
   return ret;
}

//
// Lth_strentdup
//
// Duplicates a string into a new string entity.
//
__str Lth_strentdup(char const *s)
{
   ACS_BeginPrint();
   Lth_PrintString(s);
   return ACS_EndStrParam();
}

//
// Lth_strlocal
//
__str Lth_strlocal(__str s)
{
   ACS_BeginPrint();
   ACS_PrintLocalized(s);
   return ACS_EndStrParam();
}

//
// Lth_strealoc
//
// Reallocates a string and replaces its contents.
//
char *Lth_strealoc(char *p, char const *s)
{
   size_t len = strlen(s);
   p = realloc(p, len + 1);
   memmove(p, s, len);
   p[len] = '\0';
   return p;
}

//
// Lth_strealoc_str
//
char *Lth_strealoc_str(char *p, __str s)
{
   size_t len = ACS_StrLen(s);
   p = realloc(p, len + 1);
   ACS_StrArsCpyToGlobalCharRange((int)p, __GDCC__Sta, 0, len, s);
   p[len] = '\0';
   return p;
}

//
// Lth_strcontains
//
bool Lth_strcontains(char const *s, char ch)
{
   for(; *s; s++) if(*s == ch) return true;
   return false;
}

//
// Lth_stricmp
//
int Lth_stricmp(char const *s1, char const *s2)
{
   for(; *s1 && *s2; s1++, s2++)
      if(toupper(*s1) != toupper(*s2))
         break;

   return (int)toupper(*s1) - (int)toupper(*s2);
}

//
// Lth_wcsdup
//
wchar_t *Lth_wcsdup(wchar_t const *s)
{
   size_t   len = wcslen(s);
   wchar_t *ret = malloc(sizeof(wchar_t) * (len + 1));
   wmemmove(ret, s, len);
   ret[len] = '\0';
   return ret;
}

//
// Lth_wcsdupstr
//
wchar_t *Lth_wcsdupstr(char const *s)
{
   size_t   len = strlen(s);
   wchar_t *ret = malloc(sizeof(wchar_t) * (len + 1));
   for(size_t i = 0; i < len; i++) ret[i] = s[i];
   ret[len] = '\0';
   return ret;
}

//
// Lth_mbslen
//
size_t Lth_mbslen(char const *s)
{
   size_t ret = 0;

   Lth_WithMbState()
      for(char const *end = s + strlen(s); s < end; ret++)
   {
      int next = mbrlen(s, end - s, &state);
      if(next < 0)
         return ret;
      s += next;
   }

   return ret;
}

//
// Lth_ceilk
//
int Lth_ceilk(accum n)
{
   union {int_k_t i; accum a;} u = {.a = n};
   if(u.i & 0xFFF1)
      return u.i &= 0xFFFF0000, u.a + 1;
   else
      return (int)u.a;
}

//
// Lth_fractk
//
accum Lth_fractk(accum n)
{
   union {int_k_t i; accum a;} u = {.a = n};
   u.i &= 0xFFFF;
   return u.a;
}

//
// Lth_PrintString
//
// Print a C string into the ACS print buffer.
//
void Lth_PrintString(char const *s)
{
   ACS_PrintGlobalCharArray((int)s, __GDCC__Sta);
}

//
// Lth_Hash_char
//
size_t Lth_Hash_char(char const *s)
{
   if(s == NULL) return 0;

   size_t ret = 0;
   while(*s)
      ret = ret * 101 + ((unsigned char)(*s++) & 0xff);
   return ret;
}

//
// Lth_Hash_str
//
size_t Lth_Hash_str(Lth__strchar *s)
{
   if(s == NULL) return 0;

   size_t ret = 0;
   while(*s)
      ret = ret * 101 + ((unsigned char)(*s++) & 0xff);
   return ret;
}

//
// Lth_Hash_wchar
//
size_t Lth_Hash_wchar(wchar_t const *s)
{
   if(s == NULL) return 0;

   size_t ret = 0;

   Lth_WithMbState()
      for(; *s; s++)
   {
      char mb[MB_CUR_MAX];
      int  conv = wcrtomb(mb, *s, &state);
      if(conv <= 0) break;
      for(int i = 0; i < conv; i++)
         ret = ret * 101 + ((unsigned char)(mb[i]) & 0xff);
   }

   return ret;
}

// EOF
