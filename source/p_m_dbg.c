// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Debug module.                                                            │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"

void P_Dbg_InitPre() {Dbg_Log(log_sys,  "%s", __func__);}
void P_Dbg_InitPst() {Dbg_Log(log_sys,  "%s", __func__);}
void P_Dbg_Init()    {Dbg_Log(log_sys,  "%s", __func__);}
void P_Dbg_ReInit()  {Dbg_Log(log_sys,  "%s", __func__);}
void P_Dbg_Quit()    {Dbg_Log(log_sys,  "%s", __func__);}
void P_Dbg_Dealloc() {Dbg_Log(log_sys,  "%s", __func__);}
void P_Dbg_TickPre() {Dbg_Log(log_sysV, "%s", __func__);}
void P_Dbg_Tick()    {Dbg_Log(log_sysV, "%s", __func__);}
void P_Dbg_TickPst() {Dbg_Log(log_sysV, "%s", __func__);}

/* EOF */
