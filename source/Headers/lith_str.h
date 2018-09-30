// Copyright © 2018 Graham Sanderson, all rights reserved.

#define L(name) LanguageV(name)

__str lstrupper(__str in);
stkcall u32 l_strhash(char __str_ars const *s);
stkcall u32 lstrhash(char const *s);
char *lstrcpy_str(char *dest, char __str_ars const *src);
int lstrcmp_str(char const *s1, char __str_ars const *s2);
stkcall __str l_strncpy(void const *s, int n);
stkcall __str scoresep(i96 num);
__str LanguageV(__str name);
__str Language(__str fmt, ...);
__str LanguageNull(__str fmt, ...);
__str StrParam(__str fmt, ...);
void StrParamBegin(__str fmt, ...);

// EOF
