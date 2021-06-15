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

#include <GDCC.h>
#include <stdio.h>

#define Stringify(s) #s
#define XStringify(s) Stringify(s)

#define L(name) LanguageV(name)
#define LC(name) LanguageVC(nil, name)

#define StrParamBegin(...) (ACS_BeginPrint(), __nprintf(__VA_ARGS__))
#define StrParam(...) (StrParamBegin(__VA_ARGS__), ACS_EndStrParam())
#define Language(...) (StrParamBegin(__VA_ARGS__), LanguageV(ACS_EndStrParam()))
#define LanguageC(...) LanguageCV(nil, __VA_ARGS__)

#define PrintChars(s, n) ACS_PrintGlobalCharRange((i32)s, __GDCC__Sta, 0, n)
#define PrintChrSt(s)    ACS_PrintGlobalCharArray((i32)s, __GDCC__Sta)

#define fast_strndup(s, n) (ACS_BeginPrint(), PrintChars(s, n), ACS_EndStrParam())
#define fast_strdup(s)     (ACS_BeginPrint(), PrintChrSt(s),    ACS_EndStrParam())

#define fast_strcpy2(s1, s2) \
   (ACS_BeginPrint(), PrintChrSt(s1), PrintChrSt(s2), ACS_EndStrParam())

#define fastmemset(p, s, c, ...) \
   statement(__with(register byte *_p = (void *)(p), \
                         _s = (s);) \
      for(register i32 _i = 0, _c = (c); _i < _c; _i++) \
         _p[_i] = _s; \
   )

#define fastmemcpy(lhs, rhs, s) \
   statement({ \
      register byte       *_lhs = (void *)(lhs); \
      register byte const *_rhs = (void *)(rhs); \
      register mem_size_t  _s   = (s); \
      while(_s--) *_lhs++ = *_rhs++; \
   })

#define fastmemmove(lhs, rhs, s) \
   statement({ \
      register byte       *_lhs = (void *)(lhs); \
      register byte const *_rhs = (void *)(rhs); \
      register mem_size_t  _s   = (s); \
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
      for(; *_dest || (*_dest = *_src); ++_dest, ++_src); \
   })

#define faststrcat2(dest, src1, src2) \
   statement({ \
      register cstr  _src1 = src1; \
      register cstr  _src2 = src2; \
      register char *_dest = dest; \
      for(; *_dest || (*_dest = *_src1); ++_dest, ++_src1); \
      for(; (*_dest = *_src2); ++_dest, ++_src2); \
   })

#define faststrcpy_str(dest, src) \
   statement({ \
      register astr  _src  = src; \
      register char *_dest = dest; \
      for(; (*_dest = *_src); ++_dest, ++_src); \
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

#define CpyStrLocal(out, st) \
   statement({ \
      ACS_BeginPrint(); \
      ACS_PrintLocalized(st); \
      str s = ACS_EndStrParam(); \
      for(i32 i = 0, l = ACS_StrLen(s); i <= l; i++) out[i] = s[i]; \
   })

stkcall i32 radix(char c);
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
stkcall cstr scoresep(i96 num);
stkcall cstr alientext(i32 num);
str LanguageV(str name);
char *LanguageVC(char *out, cstr name);
char *LanguageCV(char *out, cstr name, ...);
str LanguageNull(cstr fmt, ...);
stkcall cstr RemoveTextColors_str(astr s, i32 size);
stkcall cstr RemoveTextColors    (cstr s, i32 size);

/* EOF */
