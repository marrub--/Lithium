/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Upgrade handling.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"
#include "w_world.h"

/* Extern Objects ---------------------------------------------------------- */

extern struct upgradeinfo upgrinfo[UPGR_MAX];

/* Static Functions -------------------------------------------------------- */

static bool UpgrCanBuy(struct player *p, struct shopdef const *, void *upgr) {
   return !((struct upgrade *)upgr)->owned;
}

static void UpgrShopBuy(struct player *p, struct shopdef const *, void *upgr) {
   P_Upg_SetOwned(p, upgr);
}

static bool UpgrGive(struct player *, struct shopdef const *, void *upgr_, i32 tid) {
   struct upgrade const *upgr = upgr_;

   SetMembI(tid, sm_UpgradeId, upgr->info->key);

   switch(upgr->info->category) {
   case UC_Body: SetMembI(tid, sm_UpgradeBody, true); break;
   case UC_Weap: SetMembI(tid, sm_UpgradeWeap, true); break;
   default:      SetMembI(tid, sm_UpgradeExtr, true); break;
   }

   return true;
}

/* Extern Functions -------------------------------------------------------- */

void Upgr_MInit(void) {
   for(i32 i = 0; i < UPGR_MAX; i++) {
      struct upgradeinfo *ui = &upgrinfo[i];

      /* Set up static function pointers */
      ui->ShopBuy    = UpgrShopBuy;
      ui->ShopCanBuy = UpgrCanBuy;
      ui->ShopGive   = UpgrGive;

      /* Set up individual upgrades' function pointers */
      switch(ui->key) {
      #define Ret(n) continue;
      #define Fn_F(n, cb) ui->cb = Upgr_##n##_##cb;
      #define Fn_S(n, cb) Fn_F(n, cb)
      #include "u_func.h"
         continue;
      }
   }
}

void P_Upg_SetOwned(struct player *p, struct upgrade *upgr) {
   if(upgr->owned) return;

   upgr->owned = true;
   p->upgradesowned++;

   if(upgr->info->category == UC_Body && upgr->info->cost == 0)
      P_Upg_Toggle(p, upgr);
}

script void P_Upg_PInit(struct player *p) {
   fastmemset(p->upgrades, 0, sizeof p->upgrades[0] * countof(p->upgrades));

   for(i32 i = 0; i < UPGR_MAX; i++) {
      if(upgrinfo[i].pclass & p->pclass) {
         p->upgrades[i].available = true;
      }
   }

   for_upgrade(upgr) {
      upgr->info = &upgrinfo[_i];

      if(upgr->info->cost == 0 || dbgflag & dbgf_upgr)
         P_Upg_Buy(p, upgr, true, true);
   }

   p->upgrinit = true;
}

void P_Upg_PQuit(struct player *p) {
   fastmemset(p->upgrades, 0, sizeof p->upgrades[0] * countof(p->upgrades));

   p->upgrinit = false;
}

void P_Upg_PDeinit(struct player *p) {
   for_upgrade(upgr)
      if(upgr->active)
         upgr->wasactive = true,  P_Upg_Toggle(p, upgr);
}

void P_Upg_PMInit(struct player *p) {
   for_upgrade(upgr)
      if(upgr->wasactive)
         upgr->wasactive = false, P_Upg_Toggle(p, upgr);
}

script void P_Upg_PTick(struct player *p) {
   if(Paused)
      return;

   for_upgrade(upgr)
      if(upgr->active && upgr->info->Update)
         upgr->info->Update(p, upgr);
}

script void P_Upg_PTickPst(struct player *p) {
   for_upgrade(upgr) if(upgr->active && upgr->info->Render) {
      SetSize(320, 240);
      ClearClip();
      upgr->info->Render(p, upgr);
   }
}

void P_Upg_Enter(struct player *p) {
   for_upgrade(upgr)
      if(upgr->active && upgr->info->Enter)
         upgr->info->Enter(p, upgr);
}

bool P_Upg_CanActivate(struct player *p, struct upgrade *upgr) {
   if(!upgr->active &&
      (p->pclass == pcl_marine &&
       CheckRequires_AI  ||
       CheckRequires_WMD ||
       CheckRequires_WRD ||
       CheckRequires_RDI ||
       CheckRequires_RA) ||
      p->cbi.pruse + upgr->info->perf > cbiperf)
      return false;
   else
      return upgr->owned;
}

bool P_Upg_Toggle(struct player *p, struct upgrade *upgr) {
   if(!P_Upg_CanActivate(p, upgr)) return false;

   upgr->active = !upgr->active;

   if(upgr->active) p->cbi.pruse += upgr->info->perf;
   else             p->cbi.pruse -= upgr->info->perf;

   if(upgr->active && upgr->info->group)
      for_upgrade(other)
         if(other != upgr && other->active && other->info->group == upgr->info->group)
            P_Upg_Toggle(p, other);

   if(upgr->active) {
      if(upgr->info->Activate)
         upgr->info->Activate(p, upgr);

      p->scoremul += upgr->info->scoreadd;
   } else {
      if(upgr->info->Deactivate)
         upgr->info->Deactivate(p, upgr);

      p->scoremul -= upgr->info->scoreadd;
   }

   return true;
}

/* EOF */
