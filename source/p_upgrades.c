// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Upgrade handling.                                                        │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"
#include "w_world.h"

bool Upgr_CanBuy(struct upgrade const *upgr) {
   return !get_bit(upgr->flags, _ug_owned);
}

bool Upgr_Give(struct upgrade const *upgr, i32 tid) {
   SetMembI(tid, sm_UpgradeId, upgr->key);
   switch(upgr->category) {
   case _uc_body: SetMembI(tid, sm_UpgradeBody, true); break;
   case _uc_weap: SetMembI(tid, sm_UpgradeWeap, true); break;
   default:       SetMembI(tid, sm_UpgradeExtr, true); break;
   }
   return true;
}

#define Fn(n, cb) Upgr_##n##_##cb();
#define Case(n) break; case UPGR_##n:;

static void Upgr_Activate(i32 key) {
   switch(key) {
   #define A
   #include "u_func.h"
   }
}

static void Upgr_Deactivate(i32 key) {
   switch(key) {
   #define D
   #include "u_func.h"
   }
}

#undef Fn
#undef Case

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
   if(upgr->category == _uc_body && upgr->cost == 0) {
      P_Upg_Toggle(upgr);
   }
}

script void P_Upg_PInit(void) {
   fastmemcpy(pl.upgrades, upgrinfo, sizeof pl.upgrades);
   for(i32 i = 0; i < UPGR_MAX; ++i) {
      struct upgrade *upgr = &pl.upgrades[i];
      if(get_bit(upgr->pclass, pl.pclass)) {
         set_bit(upgr->flags, _ug_available);
         if(upgr->cost == 0 || dbgflags(dbgf_upgr)) {
            P_Upg_Buy(upgr, true, true);
         }
      }
   }
}

void P_Upg_PDeinit(void) {
   for_upgrade(upgr) {
      if(get_bit(upgr->flags, _ug_active)) {
         set_bit(upgr->flags, _ug_wasactive);
         P_Upg_Toggle(upgr);
      }
   }
}

void P_Upg_PMInit(void) {
   for_upgrade(upgr) {
      if(get_bit(upgr->flags, _ug_wasactive)) {
         dis_bit(upgr->flags, _ug_wasactive);
         P_Upg_Toggle(upgr);
      }
   }
}

#define Fn(n, cb) \
   if(get_bit(pl.upgrades[UPGR_##n].flags, _ug_available) && \
      get_bit(pl.upgrades[UPGR_##n].flags, _ug_active)) \
   { \
      Upgr_##n##_##cb(); \
   }
#define Case(n)

static script void P_Upg_pTickPst(void) {
   SetSize(320, 240);
   ClearClip();
   #define R
   #include "u_func.h"
}

script void P_Upg_PTick(void) {
   if(pl.modal >= _gui_disables_hud) {
      pl.hudenabled = false;
   } else {
      pl.hudenabled = false;
      for_upgrade(upgr) {
         if(get_bit(upgr->flags, _ug_active) && upgr->group == 10) {
            pl.hudenabled = true;
         }
      }
   }
   if(!Paused) {
      #define U
      #include "u_func.h"
   }
   P_Upg_pTickPst();
}

void P_Upg_Enter(void) {
   #define E
   #include "u_func.h"
}

#undef Fn
#undef Case

i32 P_Upg_CheckReqs(struct upgrade *upgr) {
   i32 ret = 0;
   for(i32 ureq = 0; ureq < _ur_max; ++ureq) {
      if(get_bit(upgr->requires, ureq)) {
         switch(ureq) {
         #define Req(r, cond) case r: if(!(cond)) {set_bit(ret, ureq);} break
         Req(_ur_ai,  get_bit(wl.cbiupgr, cupg_m_armorinter));
         Req(_ur_wmd, get_bit(wl.cbiupgr, cupg_m_weapninter));
         Req(_ur_wrd, get_bit(wl.cbiupgr, cupg_m_weapninte2));
         Req(_ur_rdi, get_bit(wl.cbiupgr, cupg_rdistinter));
         Req(_ur_dim, get_bit(wl.cbiupgr, cupg_dimdriver));
         Req(_ur_ra,  get_bit(pl.upgrades[UPGR_ReactArmor].flags, _ug_owned));
         #undef Req
         }
      }
   }
   return ret;
}

bool P_Upg_CanActivate(struct upgrade *upgr) {
   return
      get_bit(upgr->flags, _ug_active) ||
      (get_bit(upgr->flags, _ug_owned) &&
       !P_Upg_CheckReqs(upgr) &&
       (pl.pclass != pcl_marine ||
        pl.cbi.pruse + upgr->perf <= wl.cbiperf));
}

bool P_Upg_Toggle(struct upgrade *upgr) {
   if(!P_Upg_CanActivate(upgr)) {
      return false;
   }

   tog_bit(upgr->flags, _ug_active);
   bool on = get_bit(upgr->flags, _ug_active);

   if(on) pl.cbi.pruse += upgr->perf;
   else   pl.cbi.pruse -= upgr->perf;

   if(on && upgr->group) {
      for_upgrade(other) {
         if(other != upgr && get_bit(other->flags, _ug_active) && other->group == upgr->group) {
            P_Upg_Toggle(other);
         }
      }
   }

   if(on) {
      Upgr_Activate(upgr->key);
      pl.scoremul += upgr->scoreadd;
   } else {
      Upgr_Deactivate(upgr->key);
      pl.scoremul -= upgr->scoreadd;
   }

   return true;
}

/* EOF */
