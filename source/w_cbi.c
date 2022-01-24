/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * CBI upgrade/boss reward payouts.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "w_world.h"
#include "p_player.h"

static
i32 lmvar cbispawn;

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
};

#define GetCUpgr(pclass, num) \
   (cdefs[pclass][num].msg ? &cdefs[pclass][num] : nil)

script
void CBI_Install(i32 num) {
   if(num < 0 || num >= bossreward_max) {
      return;
   }

   ifauto(struct cupgdef const *, c, GetCUpgr(pl.pclass_b, num)) {
      set_bit(cbiupgr, c->key);

      switch(c->key) {
      case cupg_m_cpu1: cbiperf += 20; break;
      case cupg_m_cpu2: cbiperf += 40; break;
      }

      if(c->nam) {
         P_BIP_Unlock(P_BIP_NameToPage(c->nam), false);
      }
   }
}

void CBI_InstallSpawned(void) {
   for(i32 i = 0; i < bossreward_max; i++) {
      if(get_bit(cbispawn, i)) {
         CBI_Install(i);
      }
   }
   cbispawn = 0;
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "CbiItemWasSpawned")
void Sc_CbiItemWasSpawned(i32 num) {
   set_bit(cbispawn, num);
}

script_str ext("ACS") addr(OBJ "PickupCbiItem")
void Sc_PickupCbiItem(i32 num) {
   FadeFlash(0, 255, 0, 0.7, 0.5);

   ifauto(struct cupgdef const *, c, GetCUpgr(pl.pclass_b, num)) {
      str nam      = ns(lang_fmt(LANG "LOG_CBI_%s", c->msg));
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
