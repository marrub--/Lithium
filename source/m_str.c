// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Various string functions.                                                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#if defined(str_hash_impl)
   u32 ret = 0;
   for(; *s; s++) ret = *s + 101 * ret;
   return ret;

#undef str_hash_impl
#elif defined(remove_text_color_impl)
   i32 j = 0;

   if(size > sizeof tcbuf) return nil;

   for(i32 i = 0; i < size; i++) {
      if(s[i] == '\C') {
         ++i;
         if(s[i] == '[') {
            while(s[i] && s[i] != ']') ++i;
            ++i;
         } else {
            ++i;
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
#include "w_world.h"

#include <stdio.h>

#define stab_ary_bgn_x(name) noinit str name[0
#define stab_ary_ent_x(strn) + 1
#define stab_ary_end_x() ];
#define stab_x(n, s) noinit str n;
#include "m_stab.h"

alloc_aut(0) script ext("ACS") addr(lsc_strinit)
void StrInit(void) {
   static bool init;
   if(init) {
      return;
   }
   init = true;
   str *ary;
   i32 i;
   #define stab_ary_bgn_x(name) i = 0, ary = name;
   #define stab_ary_ent_x(strn) ary[i++] = s"" strn;
   #define stab_ary_end_x()
   #define stab_x(n, s) n = s"" s;
   #include "m_stab.h"
}

alloc_aut(0) stkcall
void PrintChars(cstr s, mem_size_t n) {
   ACS_PrintGlobalCharRange((i32)(s), __GDCC__Sta, 0, n);
}

alloc_aut(0) stkcall
void PrintChrSt(cstr s) {
   ACS_PrintGlobalCharArray((i32)(s), __GDCC__Sta);
}

alloc_aut(0) stkcall
i32 radix(char c) {
   /**/ if(c >= 'a' && c <= 'z') return c - 'a';
   else if(c >= 'A' && c <= 'Z') return c - 'A';
   else if(c >= '0' && c <= '9') return c - '0';
   else                          return INT_MAX;
}

alloc_aut(0) stkcall
i32 faststrtoi32_str(astr p) {
   #define strto_impl_sign 1
   #define strto_impl_type i32
   #include "m_str.c"
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
bool faststrcasestr(cstr lhs, cstr rhs) {
   mem_size_t llen = faststrlen(lhs);
   mem_size_t rlen = faststrlen(rhs);
   mem_size_t i, j, k;

   for(i = 0; i < llen; i++) {
      for(j = 0, k = i; j < rlen; j++) {
         if(ToUpper(lhs[i++]) != ToUpper(rhs[j])) {
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
   register i32 res;
   while((res = *s1 - *s2++) == 0 && *s1++ != '\0');
   return res;
}

alloc_aut(0) stkcall
i32 faststrcmp(cstr s1, cstr s2) {
   if(s1 == s2) return 0;
   register i32 res;
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
bool faststrchk(cstr s1, cstr s2) {
   if(s1 == s2) return 0;
   while(*s1 && *s1 == *s2) ++s1, ++s2;
   return *s1 == *s2;
}

alloc_aut(0) stkcall
bool faststrcasechk(cstr s1, cstr s2) {
   if(s1 == s2) return 0;
   while(*s1 && ToUpper(*s1) == ToUpper(*s2)) ++s1, ++s2;
   return ToUpper(*s1) == ToUpper(*s2);
}

alloc_aut(0) stkcall
char *faststrchr(cstr s, char c) {
   do if(*s == c) return (char *)s; while(*s++);
   return nil;
}

alloc_aut(0) stkcall
char *faststrtok(char *s, char **next, char c) {
   if(s) *next = s;
   if(!*next) return nil;
   while(**next == c) ++(*next);
   if(!**next) return nil;
   char *curr = *next;
   while(**next && **next != c) ++(*next);
   if(!**next) {*next = nil; return curr;}
   *(*next)++ = '\0';
   return curr;
}

alloc_aut(0) stkcall
cstr scoresep(score_t num) {
   noinit static
   char out[48];

   if(!num) {
      out[0] = '0';
      out[1] = '\0';
      return out;
   }

   char *outp = out + countof(out) - 1;
   i32 cnum = 0;

   while(num) {
      scorediv_t div = __div(num, (score_t)10);
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
void printscr(score_t num) {
   PrintChrSt(scoresep(num));
}

alloc_aut(0) stkcall
void printk64(k64 num) {
   ACS_PrintFixed((k32)num);
}

alloc_aut(0) stkcall
cstr alientext(i32 num) {
   noinit static
   char out[80];

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

alloc_aut(0) stkcall
str lang(str name) {
   str ret = ServCallS(sm_Localize, name);
   return ret != name ? ret : snil;
}

noinit static
char tcbuf[4096];

alloc_aut(0) stkcall
cstr tmpstr(str s) {
   if(s == snil) {
      return "(null)";
   } else {
      faststrcpy_str(tcbuf, s);
      return tcbuf;
   }
}

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
