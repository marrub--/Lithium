/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Various string functions.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "m_char.h"

#include <stdio.h>

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
   for(astr c = in; *c; c++) ACS_PrintChar(ToUpper(*c));
   return ACS_EndStrParam();
}

u32 l_strhash(astr s)
{
   StrHashImpl();
}

u32 lstrhash(cstr s)
{
   StrHashImpl();
}

char *lstrcpy_str(char *dest, astr src)
{
   for(char *i = dest; (*i = *src); ++i, ++src);
   return dest;
}

char *lstrcpy2(char *out, cstr s1, cstr s2)
{
   char *p = out;
   for(; *s1; s1++) *p++ = *s1;
   for(; *s2; s2++) *p++ = *s2;
   *p++ = '\0';
   return out;
}

char *lstrcpy3(char *out, cstr s1, cstr s2, cstr s3)
{
   char *p = out;
   for(; *s1; s1++) *p++ = *s1;
   for(; *s2; s2++) *p++ = *s2;
   for(; *s3; s3++) *p++ = *s3;
   *p++ = '\0';
   return out;
}

i32 lstrcmp_str(cstr s1, astr s2)
{
   i32 res;

   while((res = *s1 - *s2++) == 0)
      if(*s1++ == '\0') break;

   return res;
}

i32 faststrcmp(cstr s1, cstr s2)
{
   if(s1 == s2) return 0;

   i32 res;

   while((res = *s1 - *s2++) == 0)
      if(*s1++ == '\0') break;

   return res;
}

i32 faststrcasecmp(cstr s1, cstr s2)
{
   if(s1 == s2) return 0;

   i32 res;

   while((res = ToUpper(*s1) - ToUpper(*s2++)) == 0)
      if(*s1++ == '\0') break;

   return res;
}

cstr scoresep(i96 num) {
   noinit static char out[48];

   if(!num) {
      out[0] = '0';
      out[1] = '\0';
      return out;
   }

   char *outp = out + countof(out) - 1;
   i32 cnum = 0;

   while(num) {
      lldiv_t div = __div(num, 10LL);
      *--outp = div.rem + '0';
      num = div.quot;

      if(++cnum == 3) {
         *--outp = ',';
         cnum = 0;
      }
   }

   if(!cnum) outp++;

   return outp;
}

cstr alientext(i32 num) {
   noinit static char out[80];

   if(!num) {
      strcpy(out, u8"");
      return out;
   }

   char *outp = out + countof(out) - 1;
   i32 cnum = 0;

   while(num) {
      div_t div = __div(num, 10);
      *--outp = 0x80 + div.rem;
      *--outp = 0x80;
      *--outp = 0xee;
      num = div.quot;

      if(++cnum == 4) {
         *--outp = 0x8a;
         *--outp = 0x80;
         *--outp = 0xee;
         cnum = 0;
      }
   }

   if(!cnum) outp += 3;

   return outp;
}

str LanguageV(str name)
{
   ACS_BeginPrint();
   ACS_PrintLocalized(name);
   str ret = ACS_EndStrParam();

   while(ret[0] == '$')
   {
      str sub = ACS_StrMid(ret, 1, INT32_MAX);
      ACS_BeginPrint();
      ACS_PrintLocalized(sub);
      str nex = ACS_EndStrParam();
      if(sub != nex) ret = nex;
      else           break;
   }

   return ret;
}

char *LanguageVC(char *out, cstr name)
{
   noinit static char sbuf[8192];

   if(!out) out = sbuf;
   CpyStrLocal(out, l_strdup(name));

   while(out[0] == '$') CpyStrLocal(out, l_strdup(&out[1]));

   return out;
}

char *LanguageCV(char *out, cstr fmt, ...)
{
   noinit static char nbuf[256];
   va_list vl;

   va_start(vl, fmt);
   vsprintf(nbuf, fmt, vl);
   va_end(vl);

   return LanguageVC(out, nbuf);
}

str LanguageNull(cstr fmt, ...)
{
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   str name = ACS_EndStrParam();
   str alias = LanguageV(name);

   return name == alias ? snil : alias;
}

/* EOF */
