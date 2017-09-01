// Copyright © 2017 Graham Sanderson, all rights reserved.
// vim: columns=110
#ifndef CPK1_UPGRADES_H
#define CPK1_UPGRADES_H

#include "lith_upgrades.h"
#include "cpk1_upgradenames.h"

#define Cpk1_CastUData(u) ((struct cpk1_upgr_data *)(u)->dataptr)
#define UData_HeadsUpDis3(u) (Cpk1_CastUData(u)->u01)

#define A(n)                       void Upgr_##n##_Activate  (struct player *p, upgrade_t *upgr);
#define D(n)                       void Upgr_##n##_Deactivate(struct player *p, upgrade_t *upgr);
#define U(n) [[__call("ScriptS")]] void Upgr_##n##_Update    (struct player *p, upgrade_t *upgr);
#define E(n)                       void Upgr_##n##_Enter     (struct player *p, upgrade_t *upgr);
#define R(n)                       void Upgr_##n##_Render    (struct player *p, upgrade_t *upgr);
#include "cpk1_upgradefuncs.h"

struct upgr_data_HeadsUpDis3 {
   double scorei;
};

struct cpk1_upgr_data {
   struct upgr_data_HeadsUpDis3 u01;
};

#endif
