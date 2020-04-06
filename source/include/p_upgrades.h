/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Upgrade entry points and utilities.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef p_upgrades_h
#define p_upgrades_h

#include "common.h"
#include "u_names.h"
#include "u_data.h"
#include "p_shopdef.h"
#include "m_list.h"

#define P_Upg_Buy(p, upgr, ...) \
   P_Shop_Buy(p, &(upgr)->info->shopdef, (upgr), cLANG "UPGRADE_TITLE_%S", __VA_ARGS__)

#define for_upgrade(name) \
   for(i32 _i = 0; _i < p->upgrmax; _i++) \
      __with(struct upgrade *name = &p->upgrades[_i];)

#define CheckRequires(a1, a2) (upgr->info->requires & a1 && !(a2))
#define CheckRequires_AI  CheckRequires(UR_AI,  cbiupgr[cupg_armorinter])
#define CheckRequires_WMD CheckRequires(UR_WMD, cbiupgr[cupg_weapninter])
#define CheckRequires_WRD CheckRequires(UR_WRD, cbiupgr[cupg_weapninte2])
#define CheckRequires_RDI CheckRequires(UR_RDI, cbiupgr[cupg_rdistinter])
#define CheckRequires_RA  CheckRequires(UR_RA,  p->upgrades[UPGR_ReactArmor].owned)

/* Extern Functions -------------------------------------------------------- */

/* These are included here so the compiler may check the function signatures. */
#define Fn_F(n, cb) stkcall void Upgr_##n##_##cb(struct player *p, struct upgrade *upgr);
#define Fn_S(n, cb) script  void Upgr_##n##_##cb(struct player *p, struct upgrade *upgr);
#include "u_func.h"

script void P_Upg_PInit(struct player *p);
void P_Upg_PQuit(struct player *p);
void P_Upg_PDeinit(struct player *p);
void P_Upg_PMInit(struct player *p);

void P_Upg_Enter(struct player *p);

#endif
