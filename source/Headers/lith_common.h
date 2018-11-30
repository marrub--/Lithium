// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifndef LITH_COMMON_H
#define LITH_COMMON_H

// unfortunate early project decisions.
#pragma GDCC FIXED_LITERAL ON
#pragma GDCC STRENT_LITERAL ON

#include <ACS_ZDoom.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "lith_types.h"
#include "lith_memory.h"
#include "lith_drawing.h"
#include "lith_math.h"
#include "lith_str.h"

#define ifw(decl, ...) __with(decl;) if(__VA_ARGS__)
#define ifauto(type, name, ...) ifw(type name = (__VA_ARGS__), name)

#define countof(a) (sizeof(a) / sizeof(*(a)))
#define swap(t, a, b) do {t _tmp = a; a = b; b = _tmp;} while(0)

#define IsSmallNumber(x) ((x) > -0.001 && (x) < 0.001)
#define CloseEnough(x, y) (IsSmallNumber(x - y))

#define LogDebug(level, ...) \
   do if(ACS_GetCVar("__lith_debug_level") & (level)) \
      Log(#level ": " __VA_ARGS__); \
   while(0)

#define TickerT(t, on, off) ((ACS_Timer() % 35) < (t) ? (on) : (off))
#define Ticker(on, off) (TickerT(17, on, off))

#define SCallI ACS_ScriptCall
#define SCallK ACS_ScriptCallFixed
#define SCallS ACS_ScriptCallString

#define GetMembI ACS_GetUserVariable
#define GetMembK ACS_GetUserVariableFixed
#define GetMembS ACS_GetUserVariableString

#define SetMembI ACS_SetUserVariable
#define SetMembK ACS_SetUserVariableFixed

#define GetPropI ACS_GetActorProperty
#define GetPropK ACS_GetActorPropertyFixed
#define GetPropS ACS_GetActorPropertyString

#define SetPropI ACS_SetActorProperty
#define SetPropK ACS_SetActorPropertyFixed
#define SetPropS ACS_SetActorPropertyString

#define GetX ACS_GetActorX
#define GetY ACS_GetActorY
#define GetZ ACS_GetActorZ

#define ServCallI(...) SCallI("Lith_HERMES", __VA_ARGS__)
#define ServCallK(...) SCallK("Lith_HERMES", __VA_ARGS__)
#define ServCallS(...) SCallS("Lith_HERMES", __VA_ARGS__)

#define DrawCallI(...) SCallI("Lith_URANUS", __VA_ARGS__)
#define DrawCallK(...) SCallK("Lith_URANUS", __VA_ARGS__)
#define DrawCallS(...) SCallS("Lith_URANUS", __VA_ARGS__)

#define Lith_IsPaused ServCallI("GetPaused")

#define Lith_PausableTick() \
   do ACS_Delay(1); \
   while(Lith_IsPaused)

#define DebugStat(...) \
   (world.dbgLevel & log_devh ? Lith_DebugStat(__VA_ARGS__) : (void)0)

#define DebugNote(...) \
   (world.dbgLevel & log_devh ? Lith_DebugNote(__VA_ARGS__) : (void)0)

// Convenience names
#define InvNum  ACS_CheckInventory
#define InvMax(arg) ACS_GetMaxInventory(0, arg)
#define InvTake ACS_TakeInventory
#define InvGive ACS_GiveInventory

// Types ---------------------------------------------------------------------|

enum {
   log_none,
   log_dev   = 1 << 0, // general debug info
   log_devh  = 1 << 1, // prints live stats to the HUD (position, angles, etc)
   log_boss  = 1 << 2, // debug info for the boss system
   log_dmon  = 1 << 3, // debug info for the monster tracker
   log_dmonV = 1 << 4, // verbose debug info for the monster tracker
   log_dlg   = 1 << 5, // debug info for the dialogue/terminal compiler
   log_bip   = 1 << 6, // debug info for the BIP
};

// Extern Functions ----------------------------------------------------------|

void Log(__str fmt, ...);

script void Lith_PrintMem(void const *data, size_t size);

stkcall void Lith_FadeFlash(int r, int g, int b, fixed amount, fixed seconds);
script optargs(1) int Lith_GetTID(int tid, int ptr);
script optargs(1) int Lith_GetPlayerNumber(int tid, int ptr);
script optargs(1) bool Lith_ValidPointer(int tid, int ptr);
script optargs(2) bool Lith_SetPointer(int tid, int ptr, int assign, int tid2, int ptr2, int flags);
stkcall int Lith_CheckActorInventory(int tid, __str item);
stkcall void Lith_GiveActorInventory(int tid, __str item, int amount);
stkcall void Lith_TakeActorInventory(int tid, __str item, int amount);
stkcall void Lith_SetActorInventory (int tid, __str item, int amount);
void Lith_DebugStat(__str fmt, ...);
void Lith_DebugNote(__str fmt, ...);

// Address Space Definitions -------------------------------------------------|

// This doesn't actually cause anything bad to happen. ZDoom expects that all
// modules know the exact size of every array, even if they don't use them
// at all and even account for that. So, we hide this to hide warnings.
#ifndef EXTERNAL_CODE
__addrdef extern __mod_arr lmvar;
__addrdef extern __hub_arr lwvar;
#endif

// Extern Objects ------------------------------------------------------------|

#ifndef EXTERNAL_CODE
extern __str dbgstat[],  dbgnote[];
extern int   dbgstatnum, dbgnotenum;
#endif

#endif
