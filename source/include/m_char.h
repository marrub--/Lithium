/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * ASCII character classification macros.
 *
 * ---------------------------------------------------------------------------|
 */

local bool IsGraph(i32 c) {return c >  ' ' && c <= '~';}
local bool IsPrint(i32 c) {return c >= ' ' && c <= '~';}
local bool IsPunct(i32 c) {return c >= '!' && c <= '/' ||
                                  c >= ':' && c <= '@' ||
                                  c >= '[' && c <= '`' ||
                                  c >= '{' && c <= '~';}
local bool IsSpace(i32 c) {return c >= 0x9 && c <= 0xD ||
                                  c == ' ';}
local bool IsBlank(i32 c) {return c == ' ' || c == 0x9;}
local bool IsDigit(i32 c) {return c >= '0' && c <= '9';}
local bool IsXDigi(i32 c) {return c >= 'a' && c <= 'f' ||
                                  c >= 'A' && c <= 'F' ||
                                  IsDigit(c);}
local bool IsLower(i32 c) {return c >= 'a' && c <= 'z';}
local bool IsUpper(i32 c) {return c >= 'A' && c <= 'Z';}
local bool IsAlpha(i32 c) {return IsLower(c) || IsUpper(c);}
local bool IsAlNum(i32 c) {return IsAlpha(c) || IsDigit(c);}
local bool IsIdent(i32 c) {return IsAlNum(c) || c == '_';}
local bool IsNumId(i32 c) {return IsIdent(c) || c == '.';}
local bool IsCntrl(i32 c) {return !IsPrint(c);}
local i32  ToUpper(i32 c) {return IsLower(c) ? c - ('a' - 'A') : c;}
local i32  ToLower(i32 c) {return IsUpper(c) ? c + ('a' - 'A') : c;}

/* EOF */
