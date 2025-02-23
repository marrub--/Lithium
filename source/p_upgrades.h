// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Upgrade entry points and utilities.                                      │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef p_upgrades_h
#define p_upgrades_h

#include "m_engine.h"
#include "u_names.h"
#include "u_data.h"
#include "p_shopdef.h"
#include "m_list.h"

#define P_Upg_Buy(upgr, ...) \
   P_Shop_Buy(&(upgr)->shopdef, LANG "UPGRADE_TITLE_", __VA_ARGS__)

#define for_upgrade(name) \
   for(i32 _i = 0; _i < UPGR_MAX; _i++) \
      if(get_bit(pl.upgrades[_i].flags, _ug_available)) \
         __with(struct upgrade *name = &pl.upgrades[_i];)

extern struct upgrade upgrinfo[UPGR_MAX];

#define A
#define D
#define U
#define E
#define R
#define Case(n)
#define Fn(n, cb) Decl_##cb void Upgr_##n##_##cb(void);
#define Decl_Activate
#define Decl_Deactivate
#define Decl_Update script
#define Decl_Enter
#define Decl_Render
#include "u_func.h"
#undef Fn
#undef Case

script void P_Upg_PInit(void);
void P_Upg_PDeinit(void);
void P_Upg_PMInit(void);

void P_Upg_Enter(void);

bool Upgr_CanBuy(struct upgrade const *upgr);
bool Upgr_Give(struct upgrade const *upgr, i32 tid);

#endif
