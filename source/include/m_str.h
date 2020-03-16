/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
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

#define L(name) LanguageV(name)
#define LC(name) LanguageVC(nil, name)

#define StrParamBegin(...) (ACS_BeginPrint(), __nprintf(__VA_ARGS__))
#define StrParam(...) (StrParamBegin(__VA_ARGS__), ACS_EndStrParam())
#define Language(...) (StrParamBegin(__VA_ARGS__), LanguageV(ACS_EndStrParam()))
#define LanguageC(...) LanguageCV(nil, __VA_ARGS__)

#define PrintChars(s, n) ACS_PrintGlobalCharRange((i32)s, __GDCC__Sta, 0, n)
#define PrintChrSt(s)    ACS_PrintGlobalCharArray((i32)s, __GDCC__Sta)

#define l_strndup(s, n) (ACS_BeginPrint(), PrintChars(s, n), ACS_EndStrParam())
#define l_strdup(s)     (ACS_BeginPrint(), PrintChrSt(s),    ACS_EndStrParam())

#define fastmemset(p, s, c, ...) \
   for(i32 _i = 0; _i < (c); _i++) ((byte __VA_ARGS__ *)(p))[_i] = s;

stkcall str l_strupper(str in);
stkcall u32 l_strhash(astr s);
stkcall u32 lstrhash(cstr s);
stkcall char *lstrcpy_str(char *dest, astr src);
stkcall char *lstrcpy2(char *out, cstr s1, cstr s2);
stkcall i32 lstrcmp_str(cstr s1, astr s2);
stkcall i32 faststrcmp(cstr s1, cstr s2);
stkcall i32 faststrcasecmp(cstr s1, cstr s2);
stkcall cstr scoresep(i96 num);
str LanguageV(str name);
str LanguageNull(cstr fmt, ...);
char *LanguageVC(char *out, cstr name);
char *LanguageCV(char *out, cstr name, ...);

/* EOF */
