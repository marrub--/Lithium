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

#include "m_char.h"

stkoff bool IsGraph(i32 c) {return c >  ' ' && c <= '~';}
stkoff bool IsPrint(i32 c) {return c >= ' ' && c <= '~';}
stkoff bool IsPunct(i32 c) {return c >= '!' && c <= '/' ||
                                   c >= ':' && c <= '@' ||
                                   c >= '[' && c <= '`' ||
                                   c >= '{' && c <= '~';}
stkoff bool IsSpace(i32 c) {return c >= 0x9 && c <= 0xD ||
                                   c == ' ';}
stkoff bool IsBlank(i32 c) {return c == ' ' || c ==   9;}
stkoff bool IsDigit(i32 c) {return c >= '0' && c <= '9';}
stkoff bool IsXDigi(i32 c) {return c >= 'a' && c <= 'f' ||
                                   c >= 'A' && c <= 'F' ||
                                   c >= '0' && c <= '9';}
stkoff bool IsLower(i32 c) {return c >= 'a' && c <= 'z';}
stkoff bool IsUpper(i32 c) {return c >= 'A' && c <= 'Z';}
stkoff bool IsAlpha(i32 c) {return c >= 'a' && c <= 'z' ||
                                   c >= 'A' && c <= 'Z';}
stkoff bool IsAlNum(i32 c) {return c >= 'a' && c <= 'z' ||
                                   c >= 'A' && c <= 'Z' ||
                                   c >= '0' && c <= '9';}
stkoff bool IsIdent(i32 c) {return c >= 'a' && c <= 'z' ||
                                   c >= 'A' && c <= 'Z' ||
                                   c >= '0' && c <= '9' ||
                                   c == '_';}
stkoff bool IsNumId(i32 c) {return c >= 'a' && c <= 'z' ||
                                   c >= 'A' && c <= 'Z' ||
                                   c >= '0' && c <= '9' ||
                                   c == '_' || c == '.';}
stkoff i32  ToUpper(i32 c) {return c >= 'a' && c <= 'z' ?
                                   c - ('a' - 'A') : c;}
stkoff i32  ToLower(i32 c) {return c >= 'A' && c <= 'Z' ?
                                   c + ('a' - 'A') : c;}

/* EOF */
