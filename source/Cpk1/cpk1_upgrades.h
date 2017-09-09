// Copyright © 2017 Graham Sanderson, all rights reserved.
// vim: columns=110
#ifndef CPK1_UPGRADES_H
#define CPK1_UPGRADES_H

#include "lith_upgrades.h"
#include "cpk1_upgradenames.h"

#define Cpk1_CastUData(u) ((struct cpk1_upgr_data *)(u)->dataptr)
#define UData_HeadsUpDis3(u) (Cpk1_CastUData(u)->u01)

#define Fn_F(n, cb) void Upgr_##n##_##cb(struct player *p, upgrade_t *upgr);
#define Fn_S(n, cb) [[__call("ScriptS")]] Fn_F(n, cb)
#include "cpk1_upgradefuncs.h"

struct upgr_data_HeadsUpDis3 {
   double scorei;
   fixed healthi;
   fixed armori;
};

struct cpk1_upgr_data {
   struct upgr_data_HeadsUpDis3 u01;
};

#endif
