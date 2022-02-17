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

#include <GDCC.h>
#include <stdio.h>

#define lang_fmt(...) (StrParamBegin(__VA_ARGS__), lang(ACS_EndStrParam()))
#define lang_fmt_discrim(...) (lang_fmt(__VA_ARGS__, pl.discrim) |? lang_fmt(__VA_ARGS__, ""))

#define StrParamBegin(...) (ACS_BeginPrint(), __nprintf(__VA_ARGS__))
#define StrParam(...) (StrParamBegin(__VA_ARGS__), ACS_EndStrParam())

#define PrintChars(s, n) ACS_PrintGlobalCharRange((i32)(s), __GDCC__Sta, 0, n)
#define PrintChrSt(s)    ACS_PrintGlobalCharArray((i32)(s), __GDCC__Sta)

#define fast_strndup(s, n) (ACS_BeginPrint(), PrintChars(s, n), ACS_EndStrParam())
#define fast_strdup(s)     (ACS_BeginPrint(), PrintChrSt(s),    ACS_EndStrParam())

#define fast_strcpy2(s1, s2) \
   (ACS_BeginPrint(), PrintChrSt(s1), PrintChrSt(s2), ACS_EndStrParam())

#define fastmemset(p, s, c, ...) \
   statement({ \
      register mem_byte_t *_p = (void *)(p); \
      register mem_byte_t  _s = (s); \
      for(register i32 _i = 0, _c = (c); _i < _c; ++_i) _p[_i] = _s; \
   })

#define fastmemcpy(lhs, rhs, s) \
   statement({ \
      register mem_byte_t       *_lhs = (void *)(lhs); \
      register mem_byte_t const *_rhs = (void *)(rhs); \
      register mem_size_t        _s   = (s); \
      while(_s--) *_lhs++ = *_rhs++; \
   })

#define fastmemmove(lhs, rhs, s) \
   statement({ \
      register mem_byte_t       *_lhs = (void *)(lhs); \
      register mem_byte_t const *_rhs = (void *)(rhs); \
      register mem_size_t        _s   = (s); \
      if(_lhs < _rhs) { \
         while(_s--) \
            *_lhs++ = *_rhs++; \
      } else { \
         _lhs += _s; \
         _rhs += _s; \
         while(_s--) \
            *--_lhs = *--_rhs; \
      } \
   })

#define faststrcpy(dest, src) \
   statement({ \
      register cstr  _src  = src; \
      register char *_dest = dest; \
      for(; (*_dest = *_src); ++_dest, ++_src); \
   })

#define faststrcat(dest, src) \
   statement({ \
      register cstr  _src  = src; \
      register char *_dest = dest; \
      register bool  _end  = false; \
      for(; *_src; ++_dest) if((_end = _end || !*_dest)) *_dest = *_src++; \
   })

#define faststrcat2(dest, src1, src2) \
   statement({ \
      register cstr  _src1 = src1; \
      register cstr  _src2 = src2; \
      register char *_dest = dest; \
      register bool  _end  = false; \
      for(; *_src1; ++_dest) if((_end = _end || !*_dest)) *_dest = *_src1++; \
      for(; (*_dest = *_src2); ++_dest, ++_src2); \
   })

#define faststrcpy2(dest, src1, src2) \
   statement({ \
      register cstr  _src1 = src1; \
      register cstr  _src2 = src2; \
      register char *_dest = dest; \
      for(; *_src1; ++_src1) *_dest++ = *_src1; \
      for(; *_src2; ++_src2) *_dest++ = *_src2; \
      *_dest++ = '\0'; \
   })

#define faststrcpy3(dest, src1, src2, src3) \
   statement({ \
      register cstr  _src1 = src1; \
      register cstr  _src2 = src2; \
      register cstr  _src3 = src3; \
      register char *_dest = dest; \
      for(; *_src1; ++_src1) *_dest++ = *_src1; \
      for(; *_src2; ++_src2) *_dest++ = *_src2; \
      for(; *_src3; ++_src3) *_dest++ = *_src3; \
      *_dest++ = '\0'; \
   })

#define faststrcpy_str(dest, src) \
   statement({ \
      register str   _src  = src; \
      register char *_dest = dest; \
      ACS_StrCpyToGlobalCharRange((i32)(_dest), __GDCC__Sta, 0, \
                                  ACS_StrLen(_src) + 1, _src, 0); \
   })

stkcall i32 radix(char c);
stkcall i32 faststrtoi32_str(astr p);
stkcall i32 faststrtoi32(cstr p);
stkcall i64 faststrtoi64(cstr p);
stkcall i96 faststrtoi96(cstr p);
stkcall u32 faststrtou32(cstr p);
stkcall u64 faststrtou64(cstr p);
stkcall u96 faststrtou96(cstr p);
stkcall bool faststrstr(cstr lhs, cstr rhs);
stkcall bool faststrcasestr(cstr lhs, cstr rhs);
stkcall mem_size_t faststrlen(cstr in);
stkcall str fast_strupper(str in);
stkcall u32 fast_strhash(astr s);
stkcall u32 faststrhash(cstr s);
stkcall i32 faststrcmp_str(cstr s1, astr s2);
stkcall i32 faststrcmp(cstr s1, cstr s2);
stkcall i32 faststrcasecmp(cstr s1, cstr s2);
stkcall bool faststrchk(cstr s1, cstr s2);
stkcall bool faststrcasechk(cstr s1, cstr s2);
stkcall char *faststrchr(cstr s, char c);
stkcall char *faststrtok(char *s, char **next, char c);
stkcall cstr scoresep(i96 num);
stkcall cstr alientext(i32 num);
stkcall str lang(str name);
stkcall str ns(str s);
stkcall cstr tmpstr(str s);
stkcall cstr RemoveTextColors_str(astr s, i32 size);
stkcall cstr RemoveTextColors    (cstr s, i32 size);

/* EOF */
