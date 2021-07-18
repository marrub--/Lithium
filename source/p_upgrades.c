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

/* Static Functions -------------------------------------------------------- */

static
bool UpgrCanBuy(struct shopdef const *, void *upgr) {
   return !get_bit(((struct upgrade *)upgr)->flags, _ug_owned);
}

static
void UpgrShopBuy(struct shopdef const *, void *upgr) {
   P_Upg_SetOwned(upgr);
}

static
bool UpgrGive(struct shopdef const *, void *upgr_, i32 tid) {
   struct upgrade const *upgr = upgr_;

   SetMembI(tid, sm_UpgradeId, upgr->info->key);

   switch(upgr->info->category) {
   case _uc_body: SetMembI(tid, sm_UpgradeBody, true); break;
   case _uc_weap: SetMembI(tid, sm_UpgradeWeap, true); break;
   default:       SetMembI(tid, sm_UpgradeExtr, true); break;
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

cstr Upgr_EnumToStr(i32 n) {
   switch(n) {
      #define upgrade_x(name) case UPGR_##name: return #name;
      #include "u_names.h"
   }
   return nil;
}

void P_Upg_SetOwned(struct upgrade *upgr) {
   if(get_bit(upgr->flags, _ug_owned)) return;

   set_bit(upgr->flags, _ug_owned);
   pl.upgradesowned++;

   if(upgr->info->category == _uc_body && upgr->info->cost == 0)
      P_Upg_Toggle(upgr);
}

script void P_Upg_PInit() {
   fastmemset(pl.upgrades, 0, sizeof pl.upgrades[0] * countof(pl.upgrades));

   for(i32 i = 0; i < UPGR_MAX; i++) {
      if(upgrinfo[i].pclass & pl.pclass) {
         set_bit(pl.upgrades[i].flags, _ug_available);
      }
   }

   for_upgrade(upgr) {
      upgr->info = &upgrinfo[_i];

      if(upgr->info->cost == 0
         #ifndef NDEBUG
         || dbgflags(dbgf_upgr)
         #endif
         )
         P_Upg_Buy(upgr, true, true);
   }

   pl.upgrinit = true;
}

void P_Upg_PQuit() {
   fastmemset(pl.upgrades, 0, sizeof pl.upgrades[0] * countof(pl.upgrades));

   pl.upgrinit = false;
}

void P_Upg_PDeinit() {
   for_upgrade(upgr) {
      if(get_bit(upgr->flags, _ug_active)) {
         set_bit(upgr->flags, _ug_wasactive);
         P_Upg_Toggle(upgr);
      }
   }
}

void P_Upg_PMInit() {
   for_upgrade(upgr) {
      if(get_bit(upgr->flags, _ug_wasactive)) {
         dis_bit(upgr->flags, _ug_wasactive);
         P_Upg_Toggle(upgr);
      }
   }
}

script void P_Upg_PTick() {
   if(pl.modal >= _gui_disables_hud) {
      pl.hudenabled = false;
   } else {
      pl.hudenabled = false;

      for_upgrade(upgr) {
         if(get_bit(upgr->flags, _ug_active) && upgr->info->group == 10) {
            pl.hudenabled = true;
         }
      }
   }

   if(Paused)
      return;

   for_upgrade(upgr)
      if(get_bit(upgr->flags, _ug_active) && upgr->info->Update)
         upgr->info->Update(upgr);
}

script void P_Upg_PTickPst() {
   for_upgrade(upgr) {
      if(get_bit(upgr->flags, _ug_active) && upgr->info->Render) {
         SetSize(320, 240);
         ClearClip();
         upgr->info->Render(upgr);
      }
   }
}

void P_Upg_Enter() {
   for_upgrade(upgr)
      if(get_bit(upgr->flags, _ug_active) && upgr->info->Enter)
         upgr->info->Enter(upgr);
}

bool P_Upg_CanActivate(struct upgrade *upgr) {
   return
      !RequiresButDontHave_AI  &&
      !RequiresButDontHave_WMD &&
      !RequiresButDontHave_WRD &&
      !RequiresButDontHave_RDI &&
      !RequiresButDontHave_RA  &&
      (get_bit(upgr->flags, _ug_owned) ||
       get_bit(upgr->flags, _ug_active)) &&
      (pl.pclass != pcl_marine ||
       pl.cbi.pruse + upgr->info->perf <= cbiperf);
}

bool P_Upg_Toggle(struct upgrade *upgr) {
   if(!P_Upg_CanActivate(upgr))
      return false;

   tog_bit(upgr->flags, _ug_active);
   bool on = get_bit(upgr->flags, _ug_active);

   if(on) pl.cbi.pruse += upgr->info->perf;
   else   pl.cbi.pruse -= upgr->info->perf;

   if(on && upgr->info->group)
      for_upgrade(other)
         if(other != upgr && get_bit(other->flags, _ug_active) && other->info->group == upgr->info->group)
            P_Upg_Toggle(other);

   if(on) {
      if(upgr->info->Activate)
         upgr->info->Activate(upgr);

      pl.scoremul += upgr->info->scoreadd;
   } else {
      if(upgr->info->Deactivate)
         upgr->info->Deactivate(upgr);

      pl.scoremul -= upgr->info->scoreadd;
   }

   return true;
}

/* EOF */
