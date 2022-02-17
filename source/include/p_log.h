// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Action logging functions.                                                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef p_log_h
#define p_log_h

#include "m_vec.h"

str RandomName(i32 id);

void P_Log_Both(i32 levl, cstr fmt, ...); /* log to HUD and full log */
void P_Log_HUDs(i32 levl, cstr fmt, ...); /* log to HUD only */
void P_Log_Full(          cstr fmt, ...); /* log to full log only */
void P_Log_Entry(void);

script void P_Log(i32 cr, i32 x, i32 yy);

#endif
