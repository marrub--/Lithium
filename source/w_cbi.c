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
i32 lmvar cbispawn[cupg_max], cbispawniter;

struct cupgdef
{
   i32  pclass;
   i32  key;
   cstr msg;
   cstr nam;
};

static
struct cupgdef const cdefs[] = {
   {pM, cupg_weapninter, "MWeapnInter", "WeapnInter"},
   {pM, cupg_weapninte2, "MWeapnInte2", "WeapnInte2"},
   {pM, cupg_armorinter, "MArmorInter", "ArmorInter"},
   {pM, cupg_hasupgr1,   "MUpgr1",      "CBIUpgr1"  },
   {pM, cupg_hasupgr2,   "MUpgr2",      "CBIUpgr2"  },
   {pM, cupg_rdistinter, "MRDistInter"              },

   {pC, cupg_c_slot3spell, "CSlot3Spell", "Feuer"   },
   {pC, cupg_c_slot4spell, "CSlot4Spell", "Rend"    },
   {pC, cupg_c_slot5spell, "CSlot5Spell", "Hulgyon" },
   {pC, cupg_c_slot6spell, "CSlot6Spell", "StarShot"},
   {pC, cupg_c_slot7spell, "CSlot7Spell", "Cercle"  },
   {pC, cupg_c_rdistinter, "CRDistInter"            },
};

struct cupgdef const *GetCUpgr(i32 pclass, i32 num)
{
   for(i32 i = 0; i < countof(cdefs); i++) {
      struct cupgdef const *c = &cdefs[i];
      if(c->pclass & pclass && c->key == num)
         return c;
   }
   return nil;
}

script
void CBI_Install(i32 num)
{
   if(num < 0 || num >= cupg_max || cbiupgr[num]) return;

   cbiupgr[num] = true;

   switch(num) {
      case cupg_hasupgr1: cbiperf += 20; break;
      case cupg_hasupgr2: cbiperf += 40; break;
   }

   pl.setActivator();

   ifauto(struct cupgdef const *, c, GetCUpgr(pl.pclass, num)) {
      if(c->nam) {
         P_BIP_Unlock(P_BIP_NameToPage(c->nam), false);
      }
   }
}

void CBI_InstallSpawned(void)
{
   for(i32 i = 0; i < cbispawniter; i++)
      CBI_Install(cbispawn[i]);
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "CBIItemWasSpawned")
void Sc_CBIItemWasSpawned(i32 num)
{
   cbispawn[cbispawniter++] = num;
}

script_str ext("ACS") addr(OBJ "PickupCBIItem")
void Sc_PickupCBIItem(i32 num)
{
   FadeFlash(0, 255, 0, 0.7, 0.5);

   ifauto(struct cupgdef const *, c, GetCUpgr(pl.pclass, num)) {
      if(c->msg) {
         str nam = ns(lang_fmt(LANG "LOG_CBI_%s", c->msg));
         i32 itemdisp = CVarGetI(sc_player_itemdisp);
         if(itemdisp & _itm_disp_pop) {
            P_ItemPopup(nam, GetX(0), GetY(0), GetZ(0));
         }
         if(itemdisp & _itm_disp_log) {
            pl.logB(1, tmpstr(lang(sl_log_cbi)), nam);
         }
      }
   }

   CBI_Install(num);
}

/* EOF */
