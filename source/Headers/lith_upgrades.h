// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifndef LITH_UPGRADES_H
#define LITH_UPGRADES_H

#include "lith_common.h"
#include "lith_upgradenames.h"
#include "lith_upgradedata.h"
#include "lith_shopdef.h"
#include "lith_list.h"

#define Lith_UpgrBuy(p, upgr, ...) \
   p->buy(&(upgr)->info->shopdef, (upgr), "LITH_UPGRADE_TITLE_%S", __VA_ARGS__)

#define ForUpgrade(name) \
   for(int _i = 0; _i < p->upgrmax; _i++) \
      __with(upgrade_t *name = &p->upgrades[_i];)

#define CheckRequires(a1, a2) (upgr->info->requires & a1 && !(a2))
#define CheckRequires_AI  CheckRequires(UR_AI,  world.cbiupgr[cupg_armorinter])
#define CheckRequires_WMD CheckRequires(UR_WMD, world.cbiupgr[cupg_weapninter])
#define CheckRequires_WRD CheckRequires(UR_WRD, world.cbiupgr[cupg_weapninte2])
#define CheckRequires_RDI CheckRequires(UR_RDI, world.cbiupgr[cupg_rdistinter])
#define CheckRequires_RA  CheckRequires(UR_RA,  p->getUpgr(UPGR_ReactArmor)->owned)

// Extern Functions ----------------------------------------------------------|

// These are included here so the compiler may check the function signatures.
#define Fn_F(n, cb) stkcall void Upgr_##n##_##cb(struct player *p, upgrade_t *upgr);
#define Fn_S(n, cb) script  void Upgr_##n##_##cb(struct player *p, upgrade_t *upgr);
#include "lith_upgradefuncs.h"


script void Lith_PlayerInitUpgrades(struct player *p);
void Lith_PlayerDeallocUpgrades(struct player *p);
void Lith_PlayerDeinitUpgrades(struct player *p);
void Lith_PlayerReinitUpgrades(struct player *p);

void Lith_PlayerEnterUpgrades(struct player *p);

upgradeinfo_t *Lith_UpgradeRegister(upgradeinfo_t const *upgr);

// Extern Objects ------------------------------------------------------------|

#ifndef EXTERNAL_CODE
extern upgradeinfo_t const upgrinfobase[UPGR_BASE_MAX];
#endif

#endif

