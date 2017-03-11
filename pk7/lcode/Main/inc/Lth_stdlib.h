//-----------------------------------------------------------------------------
//
// Copyright © 2016-2017 Project Golan
//
// See "LICENSE.lithos3" for more information.
//
//-----------------------------------------------------------------------------
//
// Miscallaneous library functions.
//
//-----------------------------------------------------------------------------

#ifndef lithos3__Lth_stdlib_h
#define lithos3__Lth_stdlib_h

#include "Lth_types.h"

#include <stdfix.h>
#include <stdio.h>
#include <ctype.h>
#include <wchar.h>
#include <stdbool.h>

#define Lth_isidenti(c) \
   (isalnum(c) || c == '_' || c == '$' || c == '\'' || c > 0x80)

#define Lth_integk(n) ((int_k_t)(n))


// Extern Functions ----------------------------------------------------------|

// file
int Lth_fpeekc(FILE *fp);

// str
char *Lth_strdup(char const *s);
char *Lth_strdup_str(__str s);
__str Lth_strentdup(char const *s);
__str Lth_strlocal(__str s);
char *Lth_strealoc(char *p, char const *s);
char *Lth_strealoc_str(char *p, __str s);
bool  Lth_strcontains(char const *s, char ch);
int   Lth_stricmp(char const *s1, char const *s2);

// wcs
wchar_t *Lth_wcsdup(wchar_t const *s);
wchar_t *Lth_wcsdupstr(char const *s);

// mbs
size_t Lth_mbslen(char const *s);

// accum
int Lth_ceilk(accum n);
accum Lth_fractk(accum n);

// Print
void Lth_PrintString(char const *s);

// Hash
size_t Lth_Hash_char(char const *s);
size_t Lth_Hash_str(Lth__strchar *s);
size_t Lth_Hash_wchar(wchar_t const *s);

#endif//lithos3__Lth_stdlib_h
