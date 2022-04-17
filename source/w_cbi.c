// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ CBI upgrade/boss reward payouts.                                         │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "w_world.h"
#include "p_player.h"

static
i32 cbispawn;

static
struct cupgdef {
   i32  key;
   cstr msg;
   cstr nam;
} const cdefs[pcl_max_b][bossreward_max] = {
   [pcl_marine_b] = {
      {cupg_m_weapninter, "MWeapnInter", "WeapnInter"},
      {cupg_m_weapninte2, "MWeapnInte2", "WeapnInte2"},
      {cupg_m_armorinter, "MArmorInter", "ArmorInter"},
      {cupg_m_cpu1,       "MUpgr1",      "CPU1"      },
      {cupg_m_cpu2,       "MUpgr2",      "CPU2"      },
      {cupg_rdistinter,   "MRDistInter"              },
   },
   [pcl_cybermage_b] = {
      {cupg_c_slot3spell, "CSlot3Spell", "Feuer"   },
      {cupg_c_slot4spell, "CSlot4Spell", "Rend"    },
      {cupg_c_slot5spell, "CSlot5Spell", "Hulgyon" },
      {cupg_c_slot6spell, "CSlot6Spell", "StarShot"},
      {cupg_c_slot7spell, "CSlot7Spell", "Cercle"  },
      {cupg_rdistinter,   "CRDistInter"            },
   },
   [pcl_darklord_b] = {
      {cupg_d_motra,     "DMotra",     "Motra"    },
      {cupg_d_zaruk,     "DZaruk",     "Zaruk"    },
      {cupg_d_shield,    "DShield",    "Shield"   },
      {cupg_d_zakwu,     "DZakwu",     "Zakwu"    },
      {cupg_d_zikr,      "DZikr",      "Zikr"     },
      {cupg_d_dimdriver, "DDimDriver", "DimDriver"},
   },
};

#define GetCUpgr(pclass, num) \
   (cdefs[pclass][num].msg ? &cdefs[pclass][num] : nil)

script
void CBI_Install(i32 num) {
   if(num < 0 || num >= bossreward_max) {
      return;
   }

   pl.setActivator();

   ifauto(struct cupgdef const *, c, GetCUpgr(pl.pclass_b, num)) {
      set_bit(wl.cbiupgr, c->key);

      switch(c->key) {
      case cupg_m_cpu1: wl.cbiperf += 20; break;
      case cupg_m_cpu2: wl.cbiperf += 40; break;

      case cupg_c_slot3spell: InvGive(so_Feuer,    1); break;
      case cupg_c_slot4spell: InvGive(so_Rend,     1); break;
      case cupg_c_slot5spell: InvGive(so_Hulgyon,  1); break;
      case cupg_c_slot6spell: InvGive(so_StarShot, 1); break;
      case cupg_c_slot7spell: InvGive(so_Cercle,   1); break;

      #define GiveSubWeapon(bit) set_bit(pl.upgrdata.subweapons.have, bit)
      case cupg_d_zikr:  GiveSubWeapon(_subw_dart);    break;
      case cupg_d_zakwu: GiveSubWeapon(_subw_axe);     break;
      case cupg_d_zaruk: GiveSubWeapon(_subw_grenade); break;
      case cupg_d_motra: InvGive(so_Motra, 1);         break;
      case cupg_d_shield:
         pl.regenwaitmax = 20;
         pl.regenwait = 5;
         break;
      #undef GiveSubWeapon
      }

      if(c->nam) {
         P_BIP_Unlock(P_BIP_NameToPage(c->nam), false);
      }
   } else {
      Dbg_Err(_l("no upgrade "), _p(num), _l(" for "), _p(pl.pclass_b));
   }
}

void CBI_InstallSpawned(void) {
   for(i32 i = 0; i < bossreward_max; i++) {
      if(get_bit(cbispawn, i)) {
         CBI_Install(i);
      }
   }
}

script_str ext("ACS") addr(OBJ "CbiItemWasSpawned")
void Z_CbiItemWasSpawned(i32 num) {
   set_bit(cbispawn, num);
}

script_str ext("ACS") addr(OBJ "PickupCbiItem")
void Z_PickupCbiItem(i32 num) {
   FadeFlash(0, 255, 0, 0.7, 0.5);

   ifauto(struct cupgdef const *, c, GetCUpgr(pl.pclass_b, num)) {
      str nam      = ns(lang(fast_strdup2(LANG "LOG_CBI_", c->msg)));
      i32 itemdisp = CVarGetI(sc_player_itemdisp);
      if(itemdisp & _itm_disp_pop) {
         P_ItemPopup(nam, GetX(0), GetY(0), GetZ(0));
      }
      if(itemdisp & _itm_disp_log) {
         pl.logB(1, tmpstr(lang(sl_log_cbi)), nam);
      }
   }

   CBI_Install(num);
}

/* EOF */
