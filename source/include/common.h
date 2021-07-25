/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Common functions and debugging functions.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef common_h
#define common_h

#pragma GDCC FIXED_LITERAL ON

#include <ACS_ZDoom.h>
#include <stdarg.h>
#include <stdlib.h>

#include "m_types.h"
#include "m_memory.h"
#include "m_drawing.h"
#include "m_math.h"
#include "m_str.h"
#include "m_stab.h"
#include "m_cheat.h"
#include "m_char.h"

#define Stringify(s)  #s
#define XStringify(s) Stringify(s)

#define LineHash    (__LINE__ * FileHash)
#define LineHashStr XStringify(__LINE__) "_" XStringify(FileHash)
#define LineHashIdent_( ln, fh) ln ## _ ## fh
#define LineHashIdent__(ln, fh) LineHashIdent_(ln, fh)
#define LineHashIdent           LineHashIdent__(__LINE__, FileHash)

#define ifw(decl, ...) __with(decl;) if(__VA_ARGS__)
#define ifauto(type, name, ...) ifw(type name = (__VA_ARGS__), name)

#define countof(a) (sizeof(a) / sizeof(*(a)))
#define swap(t, a, b) statement({ t _tmp = a; a = b; b = _tmp; })

#define IsSmallNumber(x) ((x) > -0.001 && (x) < 0.001)
#define CloseEnough(x, y) (IsSmallNumber(x - y))

#ifndef NDEBUG
#define Dbg_Log(level, ...) \
   statement(if(dbglevel(level)) Log(c"" #level ": " __VA_ARGS__);)
#else
#define Dbg_Log(...)
#endif

/* !!!FIXME!!! GZDoom doesn't have a way of using PRINT_NONOTIFY yet */
#define ConsoleLog(...) ((void)0)

#define TickerT(t, on, off) ((ACS_Timer() % 35) < (t) ? (on) : (off))
#define Ticker(on, off) (TickerT(17, on, off))

#define FourCC(a, b, c, d) ((d << 24) | (c << 16) | (b << 8) | (a << 0))

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

#define CVarGetI ACS_GetCVar
#define CVarGetK ACS_GetCVarFixed
#define CVarGetS ACS_GetCVarString
#define CVarSetI ACS_SetCVar
#define CVarSetK ACS_SetCVarFixed
#define CVarSetS ACS_SetCVarString

#define GetX ACS_GetActorX
#define GetY ACS_GetActorY
#define GetZ ACS_GetActorZ

#define GetHealth(tid)       GetMembI(tid, sm_Health)
#define GetHeight(tid)       GetMembK(tid, sm_Height)
#define GetNameTag(tid)      GetPropS(tid, APROP_NameTag)
#define GetRadius(tid)       GetMembK(tid, sm_Radius)
#define GetSpecies(tid)      GetPropS(tid, APROP_Species)
#define GetViewHeight(tid)   ServCallK(sm_GetViewHeight)
#define GetAttackHeight(tid) ServCallK(sm_GetAttackHeight)

#define SetAlpha(tid, x)            SetPropK(tid, APROP_Alpha,            x)
#define SetDamage(tid, x)           SetPropI(tid, APROP_Damage,           x)
#define SetDamageMultiplier(tid, x) SetPropK(tid, APROP_DamageMultiplier, x)
#define SetFriction(tid, x)         SetPropK(tid, APROP_Friction,         x)
#define SetGravity(tid, x)          SetPropK(tid, APROP_Gravity,          x)
#define SetHealth(tid, x)           SetPropI(tid, APROP_Health,           x)
#define SetNameTag(tid, x)          SetPropS(tid, APROP_NameTag,          x)
#define SetSpawnHealth(tid, x)      SetPropI(tid, APROP_SpawnHealth,      x)
#define SetSpecies(tid, x)          SetPropS(tid, APROP_Species,          x)
#define SetViewHeight(tid, x)       SetPropK(tid, APROP_ViewHeight,       x)

#define Paused ServCallI(sm_GetPaused)
#define PausableTick() do ACS_Delay(1); while(Paused)
#define WaitPause() while(Paused) ACS_Delay(1)

#define has_status(fx) ServCallI(sm_HasStatFx, fx)
#define add_status(fx) ServCallI(sm_AddStatFx, fx)
#define rem_status(fx) ServCallI(sm_RemStatFx, fx)

#define Spf s""

#define CVAR  "lith_"
#define DCVAR "__lith_"
#define LANG  "LITH_"
#define OBJ   "Lith_"

#define sCVAR  Spf CVAR
#define sDCVAR Spf DCVAR
#define sLANG  Spf LANG
#define sOBJ   Spf OBJ

#define ServCallI(...) SCallI(so_Serv, __VA_ARGS__)
#define ServCallK(...) SCallK(so_Serv, __VA_ARGS__)
#define ServCallS(...) SCallS(so_Serv, __VA_ARGS__)

#define DrawCallI(...) SCallI(so_Draw, __VA_ARGS__)
#define DrawCallK(...) SCallK(so_Draw, __VA_ARGS__)
#define DrawCallS(...) SCallS(so_Draw, __VA_ARGS__)

#ifndef NDEBUG
#define Dbg_Stat(...) \
   (dbglevel(log_devh) ? Dbg_Stat_Impl(__VA_ARGS__) : (void)0)

#define Dbg_Note(...) \
   (dbglevel(log_devh) ? Dbg_Note_Impl(__VA_ARGS__) : (void)0)

#define Dbg_Trace(n) (ACS_BeginLog(), PrintChrSt(__func__), ACS_PrintInt(n), ACS_EndLog())
#else
#define Dbg_Stat(...)
#define Dbg_Note(...)
#endif

#define InvGive ACS_GiveInventory
#define InvMax(arg) ACS_GetMaxInventory(0, arg)
#define InvNum  ACS_CheckInventory
#define InvTake ACS_TakeInventory

/* Types ------------------------------------------------------------------- */

#ifndef NDEBUG
enum {
   log_dev,   /* general debug info */
   log_devh,  /* prints live stats to the HUD (position, angles, etc) */
   log_boss,  /* debug info for the boss system */
   log_dmon,  /* debug info for the monster tracker */
   log_dmonV, /* verbose debug info for the monster tracker */
   log_dlg,   /* debug info for the dialogue/terminal compiler */
   log_bip,   /* debug info for the BIP */
   log_sys,   /* meta debug info */
   log_sysV,  /* tick info */
   log_dpl,   /* dynamic stack usage */
   log_save,  /* save data */
};

enum {
   dbgf_ammo,
   dbgf_bip,
   dbgf_gui,
   dbgf_items,
   dbgf_score,
   dbgf_upgr,
};
#endif

/* Extern Functions -------------------------------------------------------- */

void FadeFlash(i32 r, i32 g, i32 b, k32 amount, k32 seconds);
script optargs(1) i32 PtrTID(i32 tid, i32 ptr);
script optargs(1) i32 PtrPlayerNumber(i32 tid, i32 ptr);
script optargs(1) bool PtrValid(i32 tid, i32 ptr);
script optargs(2) bool PtrSet(i32 tid, i32 ptr, i32 assign, i32 tid2, i32 ptr2, i32 flags);
i32  PtrInvNum(i32 tid, str item);
void PtrInvGive(i32 tid, str item, i32 amount);
void PtrInvTake(i32 tid, str item, i32 amount);
void PtrInvSet (i32 tid, str item, i32 amount);
#ifndef NDEBUG
void Dbg_Stat_Impl(cstr fmt, ...);
void Dbg_Note_Impl(cstr fmt, ...);
void Dbg_PrintMem(void const *data, mem_size_t size);
void Log(cstr fmt, ...);
#endif

/* Extern Objects ---------------------------------------------------------- */

#ifndef NDEBUG
extern str dbgstat[],  dbgnote[];
extern i32 dbgstatnum, dbgnotenum;

#define dbglevel(level) get_bit(CVarGetI(sc_debug_level), level)
#define dbgflags(flags) get_bit(CVarGetI(sc_debug_flags), flags)
#define dbglevel_any()  !!CVarGetI(sc_debug_level)
#define dbgflags_any()  !!CVarGetI(sc_debug_flags)
#endif

#endif
