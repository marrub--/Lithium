// Copyright © 2018 Graham Sanderson, all rights reserved.

#define L(name) LanguageV(name)

__str StrUpper(__str in);
stkcall u32 StrHash(char __str_ars const *s);
stkcall u32 CStrHash(char const *s);
char *Lith_strcpy_str(char *dest, char __str_ars const *src);
int Lith_strcmp_str(char const *s1, char __str_ars const *s2);
stkcall __str Lith_ScoreSep(i96 num);
__str LanguageV(__str name);
__str Language(__str fmt, ...);
__str LanguageNull(__str fmt, ...);
__str StrParam(__str fmt, ...);
void StrParamBegin(__str fmt, ...);

// EOF
