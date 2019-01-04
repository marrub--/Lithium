// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#ifndef p_upgrades_h
#define p_upgrades_h

#include "common.h"
#include "u_names.h"
#include "u_data.h"
#include "p_shopdef.h"
#include "m_list.h"

#define Lith_UpgrBuy(p, upgr, ...) \
   p->buy(&(upgr)->info->shopdef, (upgr), cLANG "UPGRADE_TITLE_%S", __VA_ARGS__)

#define ForUpgrade(name) \
   for(i32 _i = 0; _i < p->upgrmax; _i++) \
      __with(struct upgrade *name = &p->upgrades[_i];)

#define CheckRequires(a1, a2) (upgr->info->requires & a1 && !(a2))
#define CheckRequires_AI  CheckRequires(UR_AI,  world.cbiupgr[cupg_armorinter])
#define CheckRequires_WMD CheckRequires(UR_WMD, world.cbiupgr[cupg_weapninter])
#define CheckRequires_WRD CheckRequires(UR_WRD, world.cbiupgr[cupg_weapninte2])
#define CheckRequires_RDI CheckRequires(UR_RDI, world.cbiupgr[cupg_rdistinter])
#define CheckRequires_RA  CheckRequires(UR_RA,  p->getUpgr(UPGR_ReactArmor)->owned)

// Extern Functions ----------------------------------------------------------|

// These are included here so the compiler may check the function signatures.
#define Fn_F(n, cb) stkcall void Upgr_##n##_##cb(struct player *p, struct upgrade *upgr);
#define Fn_S(n, cb) script  void Upgr_##n##_##cb(struct player *p, struct upgrade *upgr);
#include "u_func.h"

script void Lith_PlayerInitUpgrades(struct player *p);
void Lith_PlayerDeallocUpgrades(struct player *p);
void Lith_PlayerDeinitUpgrades(struct player *p);
void Lith_PlayerReinitUpgrades(struct player *p);

void Lith_PlayerEnterUpgrades(struct player *p);

struct upgradeinfo *Lith_UpgradeRegister(struct upgradeinfo const *upgr);

// Extern Objects ------------------------------------------------------------|

extern struct upgradeinfo const upgrinfobase[UPGR_BASE_MAX];

#endif
