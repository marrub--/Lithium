// Copyright © 2018 Alison Sanderson, all rights reserved.
#include <GDCC.h>
#include <stdio.h>

#define L(name) LanguageV(name)

#define StrParamBegin(...) \
   (ACS_BeginPrint(), \
    __nprintf_str(__VA_ARGS__))

#define StrParam(...) \
   (ACS_BeginPrint(), \
    __nprintf_str(__VA_ARGS__), \
    ACS_EndStrParam())

#define l_strncpy(s, n) \
   (ACS_BeginPrint(), \
    PrintChars(s, n), \
    ACS_EndStrParam())

#define Language(...) \
   (ACS_BeginPrint(), \
    __nprintf_str(__VA_ARGS__), \
    LanguageV(ACS_EndStrParam()))

#define PrintChars(s, n) \
    ACS_PrintGlobalCharRange((int)s, __GDCC__Sta, 0, n)

__str lstrupper(__str in);
stkcall u32 l_strhash(char __str_ars const *s);
stkcall u32 lstrhash(char const *s);
char *lstrcpy_str(char *dest, char __str_ars const *src);
int lstrcmp_str(char const *s1, char __str_ars const *s2);
stkcall __str scoresep(i96 num);
__str LanguageV(__str name);
__str LanguageNull(__str fmt, ...);

// EOF
