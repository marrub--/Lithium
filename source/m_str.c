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
#elif defined(fmt_int_impl)
   #ifndef fmt_int_zero
   #define fmt_int_zero() *--outp = '0'
   #endif
   #ifdef fmt_int_sign
      #ifndef fmt_int_sign_c
      #define fmt_int_sign_c() *--outp = '-'
      #endif
   #endif
   #ifndef fmt_int_div_t
   #define fmt_int_div_t i32div
   #endif
   #ifndef fmt_int_base
   #define fmt_int_base 10
   #endif
   #ifndef fmt_int_c
   #define fmt_int_c(n) *--outp = '0' + n
   #endif
   #ifdef fmt_int_sep
      #ifndef fmt_int_sep_c
      #define fmt_int_sep_c() *--outp = ','
      #endif
      #ifndef fmt_int_sep_place
      #define fmt_int_sep_place 3
      #endif
      #ifndef fmt_int_sep_nc
      #define fmt_int_sep_nc 1
      #endif
   #endif
   noinit static char out[80];
   register char *outp = out + sizeof(out) - 1;
   #ifdef fmt_int_sign
   bool sgn = num < 0;
   if(sgn) num = -num;
   #endif
   if(num == 0) {
      #ifdef fmt_int_sign
      if(sgn) {
         fmt_int_sign_c();
      }
      #endif
      fmt_int_zero();
      return outp;
   }
   #ifdef fmt_int_sep
   i32 cnum = 0;
   #endif
   while(num) {
      register fmt_int_div_t div = __div(num, fmt_int_base);
      fmt_int_c(div.rem);
      num = div.quot;
      #ifdef fmt_int_sep
      if(++cnum == fmt_int_sep_place) {
         fmt_int_sep_c();
         cnum = 0;
      }
      #endif
   }
   #ifdef fmt_int_sep
   if(!cnum) outp += fmt_int_sep_nc;
   #endif
   #ifdef fmt_int_sign
   if(sgn) {
      fmt_int_sign_c();
   }
   #endif
   return outp;
#undef fmt_int_impl
#undef fmt_int_zero
#undef fmt_int_div_t
#undef fmt_int_base
#undef fmt_int_c
#undef fmt_int_sep
#undef fmt_int_sep_c
#undef fmt_int_sep_nc
#undef fmt_int_sep_place
#else
#include "m_engine.h"
#include "w_world.h"
#include "p_player.h"

#include <stdio.h>

#define stab_ary_bgn_x(name) noinit str name[0
#define stab_ary_ent_x(strn) + 1
#define stab_ary_end_x() ];
#define stab_x(n, s) noinit str n;
#define stab_language_x(n, s, f) \
   stab_x(n, s) \
   static noinit str _##n;
#include "m_stab.h"

noinit static char tcbuf[4096];

alloc_aut(0) script void StrInit(void) {
   register str *ary;
   register i32 i;
   #define stab_ary_bgn_x(name) i = 0, ary = name;
   #define stab_ary_ent_x(strn) ary[i++] = s"" strn;
   #define stab_ary_end_x()
   #define stab_x(n, s) n = s"" s;
   #define stab_language_x(n, s, f) _##n = s"" s;
   #include "m_stab.h"
   for(register str last_language = snil;;) {
      register str cur_language = CVarGetS(sc_language);
      if(last_language != cur_language) {
         #define stab_ary_bgn_x(name) i = 0, ary = name;
         #define stab_ary_ent_x(strn) ary[i++] = s"" strn;
         #define stab_ary_end_x()
         #define stab_x(n, s)
         #define stab_language_x(n, s, f) n = ns(f(_##n));
         #include "m_stab.h"
         last_language = cur_language;
      }
      ACS_Delay(35);
   }
}

stkoff void PrintStrN(cstr s, mem_size_t n) {
   ACS_PrintGlobalCharRange((i32)(s), __GDCC__Sta, 0, n);
}

stkoff void PrintStr(cstr s) {
   ACS_PrintGlobalCharArray((i32)(s), __GDCC__Sta);
}

stkoff i32 radix(char c) {
   /**/ if(c >= 'a' && c <= 'z') return c - 'a';
   else if(c >= 'A' && c <= 'Z') return c - 'A';
   else if(c >= '0' && c <= '9') return c - '0';
   else                          return INT_MAX;
}

stkoff i32 faststrtoi32_str(astr p) {
   #define strto_impl_sign 1
   #define strto_impl_type i32
   #include "m_str.c"
}

stkoff i32 faststrtoi32(cstr p) {
   #define strto_impl_sign 1
   #define strto_impl_type i32
   #include "m_str.c"
}

stkoff i64 faststrtoi64(cstr p) {
   #define strto_impl_sign 1
   #define strto_impl_type i64
   #include "m_str.c"
}

stkoff i96 faststrtoi96(cstr p) {
   #define strto_impl_sign 1
   #define strto_impl_type i96
   #include "m_str.c"
}

stkoff u32 faststrtou32(cstr p) {
   #define strto_impl_sign 0
   #define strto_impl_type u32
   #include "m_str.c"
}

stkoff u64 faststrtou64(cstr p) {
   #define strto_impl_sign 0
   #define strto_impl_type u64
   #include "m_str.c"
}

stkoff u96 faststrtou96(cstr p) {
   #define strto_impl_sign 0
   #define strto_impl_type u96
   #include "m_str.c"
}

stkoff bool faststrstr(cstr lhs, cstr rhs) {
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

stkoff bool faststrcasestr(cstr lhs, cstr rhs) {
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

stkoff mem_size_t faststrlen(cstr in) {
   mem_size_t len;
   for(len = 0; *in; ++len, ++in);
   return len;
}

stkoff str fast_strupper(str in) {
   ACS_BeginPrint();
   for(astr c = in; *c; c++) ACS_PrintChar(ToUpper(*c));
   return ACS_EndStrParam();
}

stkoff u32 fast_strhash(astr s) {
   #define str_hash_impl
   #include "m_str.c"
}

stkoff u32 faststrhash(cstr s) {
   #define str_hash_impl
   #include "m_str.c"
}

stkoff i32 faststrcmp_str(cstr s1, astr s2) {
   register i32 res;
   while((res = *s1 - *s2++) == 0 && *s1++ != '\0');
   return res;
}

stkoff i32 faststrcmp(cstr s1, cstr s2) {
   if(s1 == s2) return 0;
   register i32 res;
   while((res = *s1 - *s2++) == 0 && *s1++ != '\0');
   return res;
}

stkoff i32 faststrcasecmp(cstr s1, cstr s2) {
   if(s1 == s2) return 0;
   i32 res;
   while((res = ToUpper(*s1) - ToUpper(*s2++)) == 0 && *s1++ != '\0');
   return res;
}

stkoff bool faststrchk(cstr s1, cstr s2) {
   if(s1 == s2) return 0;
   while(*s1 && *s1 == *s2) ++s1, ++s2;
   return *s1 == *s2;
}

stkoff bool faststrcasechk(cstr s1, cstr s2) {
   if(s1 == s2) return 0;
   while(*s1 && ToUpper(*s1) == ToUpper(*s2)) ++s1, ++s2;
   return ToUpper(*s1) == ToUpper(*s2);
}

stkoff char *faststrchr(cstr s, char c) {
   do if(*s == c) return (char *)s; while(*s++);
   return nil;
}

stkoff char *faststrtok(char *s, char **next, char c) {
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

stkoff cstr scoresep(score_t num) {
   #define fmt_int_impl
   #define fmt_int_sign
   #define fmt_int_div_t scorediv
   #define fmt_int_base  (score_t)10
   #define fmt_int_sep
   #include "m_str.c"
}

stkoff cstr fmti64(i64 num) {
   #define fmt_int_impl
   #define fmt_int_sign
   #define fmt_int_div_t i64div
   #define fmt_int_base  10L
   #include "m_str.c"
}

stkoff cstr fmti96(i96 num) {
   #define fmt_int_impl
   #define fmt_int_sign
   #define fmt_int_div_t i96div
   #define fmt_int_base  10LL
   #include "m_str.c"
}

stkoff cstr fmtu32(u32 num) {
   #define fmt_int_impl
   #define fmt_int_div_t u32div
   #define fmt_int_base  10U
   #include "m_str.c"
}

stkoff cstr fmtu64(u64 num) {
   #define fmt_int_impl
   #define fmt_int_div_t u64div
   #define fmt_int_base  10UL
   #include "m_str.c"
}

stkoff cstr fmtu96(u96 num) {
   #define fmt_int_impl
   #define fmt_int_div_t u96div
   #define fmt_int_base  10ULL
   #include "m_str.c"
}

stkoff void printi64(i64 num) {
   PrintStr(fmti64(num));
}

stkoff void printi96(i96 num) {
   PrintStr(fmti96(num));
}

stkoff void printu32(u64 num) {
   PrintStr(fmtu32(num));
}

stkoff void printu64(u64 num) {
   PrintStr(fmtu64(num));
}

stkoff void printu96(u96 num) {
   PrintStr(fmtu96(num));
}

stkoff void printk64(k64 num) {
   _p((i32)num);
   _c('.');
   _p((k64fract(num) * 10000 + 0x7FFFFFFF) / 0xFFFFFFFF);
}

stkoff cstr alientext(i32 num) {
   #define AN0 u8""
   #define fmt_int_impl
   #define fmt_int_sign
   #define fmt_int_zero() \
      outp -= sizeof AN0; fastmemcpy(outp, AN0, sizeof AN0)
   #define fmt_int_c(n) \
      *--outp = 0x80 + n; \
      *--outp = 0x80; \
      *--outp = 0xee
   #define fmt_int_sep_c() \
      *--outp = 0x8a; \
      *--outp = 0x80; \
      *--outp = 0xee
   #define fmt_int_sep_place 4
   #define fmt_int_sep_nc 3
   #include "m_str.c"
}

stkoff str lang(str name) {
   str ret = ServCallS(sm_Localize, name);
   return ret != st_empty ? ret : snil;
}

stkoff cstr tmpstr(str s) {
   if(s == snil) {
      return "(null)";
   } else {
      faststrcpy_str(tcbuf, s);
      return tcbuf;
   }
}

stkoff cstr RemoveTextColors_str(astr s, i32 size) {
   #define remove_text_color_impl
   #include "m_str.c"
}

stkoff cstr RemoveTextColors(cstr s, i32 size) {
   #define remove_text_color_impl
   #include "m_str.c"
}

stkoff void printfmt(cstr s, mem_size_t n, struct fmt_arg const *args) {
   i32 which = 0;
   for(char c; (c = *s++);) {
      if(c == '{' && which < n) {
         struct fmt_arg const *arg;
         if(IsDigit(*s)) {
            i32 explicit = *s - '0';
            if(explicit < n) {
               arg = &args[explicit];
               ++s;
            } else {
               continue;
            }
         } else {
            arg = &args[which++];
         }
         if(*s == '{') {
            ACS_PrintChar('{');
            ++s;
         } else if(*s == '}') {
            ++s;
            switch(arg->tag) {
            case _fmt_cstr:
               if(!arg->precision) _p(arg->val.cs);
               else                PrintStrN(arg->val.cs, arg->precision);
               break;
            case _fmt_i32:
               if(arg->precision) {
                  register i32 digits = arg->val.i ? 0 : 1;
                  for(register i32 num = arg->val.i;
                      num && digits != arg->precision; num /= 10) {
                     ++digits;
                  }
                  for(; digits < arg->precision; ++digits) {
                     _c('0');
                  }
               }
               _p(arg->val.i);
               break;
            case _fmt_k32:
               if(!arg->precision) {
                  _p(arg->val.k);
               } else {
                  register i32 scale;
                  scale = 1;
                  for(register i32 prec = arg->precision; prec; --prec) {
                     scale *= 10;
                  }
                  _p((i32)arg->val.k);
                  _c('.');
                  _p((k32fract(arg->val.k) * scale + 0x7FFF) / 0xFFFF);
               }
               break;
            case _fmt_key: ACS_PrintBind(arg->val.s); break;
            case _fmt_str: _p(arg->val.s);            break;
            }
            continue;
         }
      }
      ACS_PrintChar(c);
   }
}
#endif
