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

void P_Dbg_InitPre(void) {Dbg_Log(log_sys,  "%s", __func__);}
void P_Dbg_InitPst(void) {Dbg_Log(log_sys,  "%s", __func__);}
void P_Dbg_Init(void)    {Dbg_Log(log_sys,  "%s", __func__);}
void P_Dbg_ReInit(void)  {Dbg_Log(log_sys,  "%s", __func__);}
void P_Dbg_Quit(void)    {Dbg_Log(log_sys,  "%s", __func__);}
void P_Dbg_Dealloc(void) {Dbg_Log(log_sys,  "%s", __func__);}
void P_Dbg_TickPre(void) {Dbg_Log(log_sysV, "%s", __func__);}
void P_Dbg_Tick(void)    {Dbg_Log(log_sysV, "%s", __func__);}
void P_Dbg_TickPst(void) {Dbg_Log(log_sysV, "%s", __func__);}

/* EOF */
