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

#include "m_char.h"

alloc_aut(0) stkcall bool IsGraph(i32 c) {return c >  ' ' && c <= '~';}
alloc_aut(0) stkcall bool IsPrint(i32 c) {return c >= ' ' && c <= '~';}
alloc_aut(0) stkcall bool IsPunct(i32 c) {return c >= '!' && c <= '/' ||
                                                 c >= ':' && c <= '@' ||
                                                 c >= '[' && c <= '`' ||
                                                 c >= '{' && c <= '~';}
alloc_aut(0) stkcall bool IsSpace(i32 c) {return c >=   9 && c <=  13 ||
                                                 c == ' ';}
alloc_aut(0) stkcall bool IsBlank(i32 c) {return c == ' ' || c ==   9;}
alloc_aut(0) stkcall bool IsDigit(i32 c) {return c >= '0' && c <= '9';}
alloc_aut(0) stkcall bool IsXDigi(i32 c) {return c >= 'a' && c <= 'f' ||
                                                 c >= 'A' && c <= 'F' ||
                                                 IsDigit(c);}
alloc_aut(0) stkcall bool IsLower(i32 c) {return c >= 'a' && c <= 'z';}
alloc_aut(0) stkcall bool IsUpper(i32 c) {return c >= 'A' && c <= 'Z';}
alloc_aut(0) stkcall bool IsAlpha(i32 c) {return IsLower(c) || IsUpper(c);}
alloc_aut(0) stkcall bool IsAlNum(i32 c) {return IsAlpha(c) || IsDigit(c);}
alloc_aut(0) stkcall bool IsIdent(i32 c) {return IsAlNum(c) || c == '_';}
alloc_aut(0) stkcall bool IsNumId(i32 c) {return IsIdent(c) || c == '.';}
alloc_aut(0) stkcall bool IsCntrl(i32 c) {return !IsPrint(c);}
alloc_aut(0) stkcall i32  ToUpper(i32 c) {return IsLower(c) ? c - ('a' - 'A') : c;}
alloc_aut(0) stkcall i32  ToLower(i32 c) {return IsUpper(c) ? c + ('a' - 'A') : c;}

/* EOF */
