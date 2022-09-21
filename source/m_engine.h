// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Engine functions.                                                        │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#if defined(cvar_x)
#define cvar_get(name) \
   (_Generic(cv.name, \
      bool: CVarGetI, \
      i32:  CVarGetI, \
      k32:  CVarGetK, \
      str:  CVarGetS)(sc_##name))
#ifndef cvar_gbl
#define cvar_gbl(ty, na)
#endif
#ifndef cvar_map
#define cvar_map(ty, na)
#endif
#ifndef cvar_tic
#define cvar_tic(ty, na)
#endif
/*
  if a setting should only update on map or game start, place it here.
*/
cvar_x(gbl, sv_autosave, i32)
cvar_x(gbl, sv_failtime, i32)
cvar_x(gbl, sv_nobossdrop, bool)
cvar_x(gbl, sv_nobosses, bool)
cvar_x(map, player_resultssound, bool)
cvar_x(map, sv_difficulty, i32)
cvar_x(map, sv_extrahard, bool)
cvar_x(map, sv_minhealth, i32)

/*
  if a cvar is checked variably more than once every tic under normal
  circumstances, place it here. if it can change in-between tics, do
  not place it here.
*/
cvar_x(tic, debug_flags, i32)
cvar_x(tic, debug_level, i32)
cvar_x(tic, player_scoredisp, i32)
cvar_x(tic, player_scoresound, bool)

#undef cvar_tic
#undef cvar_map
#undef cvar_gbl
#undef cvar_get
#undef cvar_x
#elif !defined(m_engine_h)
#define m_engine_h

#include "m_types.h"

#include <ACS_ZDoom.h>
#include <stdarg.h>
#include <stdlib.h>

#include "m_memory.h"
#include "m_drawing.h"
#include "m_math.h"
#include "m_str.h"
#include "m_stab.h"
#include "m_cheat.h"
#include "m_char.h"
#include "m_flow.h"
#include "m_dbg.h"

#define SCallV(...) (ACS_ScriptCall(__VA_ARGS__), (void)0)
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

#define GetHealth(tid)  GetMembI(tid, sm_Health)
#define GetHeight(tid)  GetMembK(tid, sm_Height)
#define GetNameTag(tid) GetPropS(tid, APROP_NameTag)
#define GetRadius(tid)  GetMembK(tid, sm_Radius)
#define GetSpecies(tid) GetPropS(tid, APROP_Species)

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

#define Paused EDataI(_edt_paused)
#define PausableTick() do ACS_Delay(1); while(Paused)
#define WaitPause() while(Paused) ACS_Delay(1)

#define has_status(fx) ServCallI(sm_HasStatFx, fx)
#define add_status(fx) ServCallV(sm_AddStatFx, fx)
#define rem_status(fx) ServCallV(sm_RemStatFx, fx)

#define ServCallV(...) SCallV(so_Serv, __VA_ARGS__)
#define ServCallI(...) SCallI(so_Serv, __VA_ARGS__)
#define ServCallK(...) SCallK(so_Serv, __VA_ARGS__)
#define ServCallS(...) SCallS(so_Serv, __VA_ARGS__)

#define DrawCallV(...) SCallV(so_Ren, __VA_ARGS__)
#define DrawCallI(...) SCallI(so_Ren, __VA_ARGS__)
#define DrawCallK(...) SCallK(so_Ren, __VA_ARGS__)
#define DrawCallS(...) SCallS(so_Ren, __VA_ARGS__)

#define InvGive     ACS_GiveInventory
#define InvMax(arg) ACS_GetMaxInventory(0, arg)
#define InvNum      ACS_CheckInventory
#define InvTake     ACS_TakeInventory

void FadeFlash(i32 r, i32 g, i32 b, k32 amount, k32 seconds);
script optargs(1) i32 PtrTID(i32 tid, i32 ptr);
script optargs(1) i32 PtrPlayerNumber(i32 tid, i32 ptr);
script optargs(1) bool PtrValid(i32 tid, i32 ptr);
script optargs(2) bool PtrSet(i32 tid, i32 ptr, i32 assign, i32 tid2, i32 ptr2, i32 flags);
i32  PtrInvNum(i32 tid, str item);
void PtrInvGive(i32 tid, str item, i32 amount);
void PtrInvTake(i32 tid, str item, i32 amount);
void PtrInvSet (i32 tid, str item, i32 amount);

struct cvars {
   #define cvar_x(ev, na, ty) ty na;
   #include "m_engine.h"
};

extern struct cvars cv;

#endif
