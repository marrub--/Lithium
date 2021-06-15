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

#if defined(str_hash_impl)
   u32 ret = 0;
   for(; *s; s++) ret = *s + 101 * ret;
   return ret;

#undef str_hash_impl
#elif defined(remove_text_color_impl)
   i32 j = 0;

   if(size > countof(tcbuf)) return nil;

   for(i32 i = 0; i < size; i++) {
      if(s[i] == '\C') {
         i++;
         if(s[i] == '[') {
            while(s[i] && s[i++] != ']');
         } else {
            i++;
         }
      }

      if(i >= size || j >= size || !s[i]) {
         break;
      }

      tcbuf[j++] = s[i];
   }

   tcbuf[j++] = '\0';

   return tcbuf;

#undef remove_text_color_impl
#elif defined(strto_impl_type)
   while(IsSpace(*p)) ++p;

   #if strto_impl_sign
   bool sign;
   switch(*p) {
   case '-': sign = true;  ++p; break;
   case '+': sign = false; ++p; break;
   default:  sign = false;      break;
   }
   #else
   if(*p == '+') ++p;
   #endif

   strto_impl_type base;
   if(*p == '0') {
      ++p;
      if(*p == 'x' || *p == 'X') {
         ++p;
         base = 16;
      } else {
         base = 8;
      }
   } else {
      base = 10;
   }

   strto_impl_type ret = 0;
   for(i32 digit; (digit = radix(*p)) < base; ++p) {
      ret = ret * base + digit;
   }
   #if strto_impl_sign
   return sign ? -ret : ret;
   #else
   return ret;
   #endif

#undef strto_impl_sign
#undef strto_impl_type
#else
#include "common.h"
#include "m_char.h"

#include <stdio.h>

#define stab_x(n, s) str const lmvar n = Spf s;
#include "m_stab.h"

alloc_aut(0) stkcall
i32 radix(char c) {
   /**/ if(c >= 'a' && c <= 'z') return c - 'a';
   else if(c >= 'A' && c <= 'Z') return c - 'A';
   else if(c >= '0' && c <= '9') return c - '0';
   else                          return INT_MAX;
}

alloc_aut(0) stkcall
i32 faststrtoi32(cstr p) {
   #define strto_impl_sign 1
   #define strto_impl_type i32
   #include "m_str.c"
}

alloc_aut(0) stkcall
i64 faststrtoi64(cstr p) {
   #define strto_impl_sign 1
   #define strto_impl_type i64
   #include "m_str.c"
}

alloc_aut(0) stkcall
i96 faststrtoi96(cstr p) {
   #define strto_impl_sign 1
   #define strto_impl_type i96
   #include "m_str.c"
}

alloc_aut(0) stkcall
u32 faststrtou32(cstr p) {
   #define strto_impl_sign 0
   #define strto_impl_type u32
   #include "m_str.c"
}

alloc_aut(0) stkcall
u64 faststrtou64(cstr p) {
   #define strto_impl_sign 0
   #define strto_impl_type u64
   #include "m_str.c"
}

alloc_aut(0) stkcall
u96 faststrtou96(cstr p) {
   #define strto_impl_sign 0
   #define strto_impl_type u96
   #include "m_str.c"
}

alloc_aut(0) stkcall
bool faststrstr(cstr lhs, cstr rhs) {
   mem_size_t llen = faststrlen(lhs);
   mem_size_t rlen = faststrlen(rhs);
   mem_size_t i, j, k;

   for(i = 0; i < llen; i++) {
      for(j = 0, k = i; j < rlen; j++) {
         if(lhs[i++] != rhs[j]) {
            i = k;
            break;
         }
      }

      if(j == rlen) {
         return true;
      }
   }

   return false;
}

alloc_aut(0) stkcall
mem_size_t faststrlen(cstr in) {
   mem_size_t len;
   for(len = 0; *in; ++len, ++in);
   return len;
}

alloc_aut(0) stkcall
str fast_strupper(str in) {
   ACS_BeginPrint();
   for(astr c = in; *c; c++) ACS_PrintChar(ToUpper(*c));
   return ACS_EndStrParam();
}

alloc_aut(0) stkcall
u32 fast_strhash(astr s) {
   #define str_hash_impl
   #include "m_str.c"
}

alloc_aut(0) stkcall
u32 faststrhash(cstr s) {
   #define str_hash_impl
   #include "m_str.c"
}

alloc_aut(0) stkcall
i32 faststrcmp_str(cstr s1, astr s2) {
   i32 res;
   while((res = *s1 - *s2++) == 0 && *s1++ != '\0');
   return res;
}

alloc_aut(0) stkcall
i32 faststrcmp(cstr s1, cstr s2) {
   if(s1 == s2) return 0;
   i32 res;
   while((res = *s1 - *s2++) == 0 && *s1++ != '\0');
   return res;
}

alloc_aut(0) stkcall
i32 faststrcasecmp(cstr s1, cstr s2) {
   if(s1 == s2) return 0;
   i32 res;
   while((res = ToUpper(*s1) - ToUpper(*s2++)) == 0 && *s1++ != '\0');
   return res;
}

alloc_aut(0) stkcall
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

alloc_aut(0) stkcall
cstr alientext(i32 num) {
   noinit static char out[80];

   if(!num) {
      fastmemcpy(out, u8"", sizeof u8"");
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

str LanguageV(str name) {
   ACS_BeginPrint();
   ACS_PrintLocalized(name);
   str ret = ACS_EndStrParam();

   while(ret[0] == '$') {
      str sub = ACS_StrMid(ret, 1, INT32_MAX);
      ACS_BeginPrint();
      ACS_PrintLocalized(sub);
      str nex = ACS_EndStrParam();
      if(sub != nex) ret = nex;
      else           break;
   }

   return ret;
}

char *LanguageVC(char *out, cstr name) {
   noinit static char sbuf[8192];

   if(!out) out = sbuf;
   CpyStrLocal(out, fast_strdup(name));

   while(out[0] == '$') CpyStrLocal(out, fast_strdup(&out[1]));

   return out;
}

char *LanguageCV(char *out, cstr fmt, ...) {
   noinit static char nbuf[256];
   va_list vl;

   va_start(vl, fmt);
   vsprintf(nbuf, fmt, vl);
   va_end(vl);

   return LanguageVC(out, nbuf);
}

str LanguageNull(cstr fmt, ...) {
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   str name = ACS_EndStrParam();
   str alias = LanguageV(name);

   return name == alias ? snil : alias;
}

noinit static char tcbuf[4096];

alloc_aut(0) stkcall
cstr RemoveTextColors_str(astr s, i32 size) {
   #define remove_text_color_impl
   #include "m_str.c"
}

alloc_aut(0) stkcall
cstr RemoveTextColors(cstr s, i32 size) {
   #define remove_text_color_impl
   #include "m_str.c"
}
#endif
