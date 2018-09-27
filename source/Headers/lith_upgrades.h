// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
// vim: columns=110
#ifndef LITH_UPGRADES_H
#define LITH_UPGRADES_H

#include "lith_common.h"
#include "lith_upgradenames.h"
#include "lith_upgradedata.h"
#include "lith_shopdef.h"
#include "lith_list.h"

#define Lith_UpgrBuy(p, upgr, ...) \
   p->buy(&(upgr)->info->shopdef, (upgr), "LITH_UPGRADE_TITLE_%S", __VA_ARGS__)

// Extern Functions ----------------------------------------------------------|

// These are included here so the compiler may check the function signatures.
#define Fn_F(n, cb) void Upgr_##n##_##cb(struct player *p, upgrade_t *upgr);
#define Fn_S(n, cb) script Fn_F(n, cb)
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

