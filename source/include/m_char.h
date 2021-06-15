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

#ifndef m_char_h
#define m_char_h
#include "m_types.h"

#define IsCntrl(c) (!IsPrint(c))

stkcall bool IsGraph(i32 c);
stkcall bool IsPrint(i32 c);
stkcall bool IsPunct(i32 c);
stkcall bool IsSpace(i32 c);
stkcall bool IsBlank(i32 c);
stkcall bool IsDigit(i32 c);
stkcall bool IsXDigi(i32 c);
stkcall bool IsLower(i32 c);
stkcall bool IsUpper(i32 c);
stkcall bool IsAlpha(i32 c);
stkcall bool IsAlNum(i32 c);
stkcall bool IsIdent(i32 c);
stkcall bool IsNumId(i32 c);
stkcall i32  ToUpper(i32 c);
stkcall i32  ToLower(i32 c);

#endif
