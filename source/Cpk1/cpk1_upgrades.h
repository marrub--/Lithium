// Copyright © 2017 Graham Sanderson, all rights reserved.
// vim: columns=110
#ifndef CPK1_UPGRADES_H
#define CPK1_UPGRADES_H

#include "lith_upgrades.h"
#include "cpk1_upgradenames.h"

#define A(n)                       void Upgr_##n##_Activate  (struct player *p, upgrade_t *upgr);
#define D(n)                       void Upgr_##n##_Deactivate(struct player *p, upgrade_t *upgr);
#define U(n) [[__call("ScriptS")]] void Upgr_##n##_Update    (struct player *p, upgrade_t *upgr);
#define E(n)                       void Upgr_##n##_Enter     (struct player *p, upgrade_t *upgr);
#define R(n)                       void Upgr_##n##_Render    (struct player *p, upgrade_t *upgr);
#include "cpk1_upgradefuncs.h"

#endif
