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
   return !get_bit(((struct upgrade *)upgr)->flags, _ug_owned);
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
   if(get_bit(upgr->flags, _ug_owned)) return;

   set_bit(upgr->flags, _ug_owned);
   p->upgradesowned++;

   if(upgr->info->category == UC_Body && upgr->info->cost == 0)
      P_Upg_Toggle(p, upgr);
}

script void P_Upg_PInit(struct player *p) {
   fastmemset(p->upgrades, 0, sizeof p->upgrades[0] * countof(p->upgrades));

   for(i32 i = 0; i < UPGR_MAX; i++) {
      if(upgrinfo[i].pclass & p->pclass) {
         set_bit(p->upgrades[i].flags, _ug_available);
      }
   }

   for_upgrade(upgr) {
      upgr->info = &upgrinfo[_i];

      if(upgr->info->cost == 0 || get_bit(dbgflag, dbgf_upgr))
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
      if(get_bit(upgr->flags, _ug_active))
         set_bit(upgr->flags, _ug_wasactive), P_Upg_Toggle(p, upgr);
}

void P_Upg_PMInit(struct player *p) {
   for_upgrade(upgr)
      if(get_bit(upgr->flags, _ug_wasactive))
         set_bit(upgr->flags, _ug_wasactive), P_Upg_Toggle(p, upgr);
}

script void P_Upg_PTick(struct player *p) {
   if(p->dlg.active) {
      p->hudenabled = false;
   } else {
      p->hudenabled = false;

      for_upgrade(upgr) {
         if(get_bit(upgr->flags, _ug_active) && upgr->info->group == UG_HUD) {
            p->hudenabled = true;
         }
      }
   }

   if(Paused)
      return;

   for_upgrade(upgr)
      if(get_bit(upgr->flags, _ug_active) && upgr->info->Update)
         upgr->info->Update(p, upgr);
}

script void P_Upg_PTickPst(struct player *p) {
   for_upgrade(upgr) {
      if(get_bit(upgr->flags, _ug_active) && upgr->info->Render) {
         SetSize(320, 240);
         ClearClip();
         upgr->info->Render(p, upgr);
      }
   }
}

void P_Upg_Enter(struct player *p) {
   for_upgrade(upgr)
      if(get_bit(upgr->flags, _ug_active) && upgr->info->Enter)
         upgr->info->Enter(p, upgr);
}

bool P_Upg_CanActivate(struct player *p, struct upgrade *upgr) {
   if(!get_bit(upgr->flags, _ug_active) &&
      (p->pclass == pcl_marine &&
       CheckRequires_AI  ||
       CheckRequires_WMD ||
       CheckRequires_WRD ||
       CheckRequires_RDI ||
       CheckRequires_RA) ||
      p->cbi.pruse + upgr->info->perf > cbiperf)
      return false;
   else
      return get_bit(upgr->flags, _ug_owned);
}

bool P_Upg_Toggle(struct player *p, struct upgrade *upgr) {
   if(!P_Upg_CanActivate(p, upgr)) return false;

   bool on = tog_bit(upgr->flags, _ug_active);

   if(on) p->cbi.pruse += upgr->info->perf;
   else   p->cbi.pruse -= upgr->info->perf;

   if(on && upgr->info->group)
      for_upgrade(other)
         if(other != upgr && get_bit(other->flags, _ug_active) && other->info->group == upgr->info->group)
            P_Upg_Toggle(p, other);

   if(on) {
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
