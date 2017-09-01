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
   p->buy(&(upgr)->info->shopdef, (upgr), "LITH_TXT_UPGRADE_TITLE_%S", __VA_ARGS__)

// Extern Functions ----------------------------------------------------------|

// These are included here so the compiler may check the function signatures.
#define A(n)                       void Upgr_##n##_Activate  (struct player *p, upgrade_t *upgr);
#define D(n)                       void Upgr_##n##_Deactivate(struct player *p, upgrade_t *upgr);
#define U(n) [[__call("ScriptS")]] void Upgr_##n##_Update    (struct player *p, upgrade_t *upgr);
#define E(n)                       void Upgr_##n##_Enter     (struct player *p, upgrade_t *upgr);
#define R(n)                       void Upgr_##n##_Render    (struct player *p, upgrade_t *upgr);
#include "lith_upgradefuncs.h"


void Lith_PlayerInitUpgrades(struct player *p);
void Lith_PlayerDeallocUpgrades(struct player *p);
void Lith_PlayerDeinitUpgrades(struct player *p);
void Lith_PlayerReinitUpgrades(struct player *p);

void Lith_PlayerUpdateUpgrades(struct player *p);
void Lith_PlayerRenderUpgrades(struct player *p);
void Lith_PlayerEnterUpgrades(struct player *p);

upgradeinfo_t const *Lith_GetUpgradeInfo(int key);
upgradeinfo_t *Lith_UpgradeRegister(upgradeinfo_t const *upgr);
void Lith_UpgradeRegisterReinit(upgr_reinit_cb_t cb);

#endif

