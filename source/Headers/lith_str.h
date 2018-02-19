// Copyright © 2018 Graham Sanderson, all rights reserved.

__str StrUpper(__str in);
[[__call("StkCall")]] u32 StrHash(char __str_ars const *s);
[[__call("StkCall")]] u32 CStrHash(char const *s);
char *Lith_strcpy_str(char *dest, char __str_ars const *src);
int Lith_strcmp_str(char const *s1, char __str_ars const *s2);
[[__call("StkCall")]] __str Lith_ScoreSep(i96 num);
__str Language(__str fmt, ...);
__str LanguageNull(__str fmt, ...);
__str StrParam(__str fmt, ...);
void StrParamBegin(__str fmt, ...);

// EOF
