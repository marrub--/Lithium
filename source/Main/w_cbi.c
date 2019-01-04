// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#if LITHIUM
#include "common.h"
#include "w_world.h"
#include "p_player.h"

static i32 lmvar cbispawn[cupg_max];
static i32 lmvar cbispawniter;

struct cupgdef
{
   i32 pclass;
   i32 key;
   str msg;
   str nam;
};

static struct cupgdef const cdefs[] = {
   {pM, cupg_weapninter, s"MWeapnInter", s"WeapnInter"},
   {pM, cupg_weapninte2, s"MWeapnInte2", s"WeapnInte2"},
   {pM, cupg_armorinter, s"MArmorInter", s"ArmorInter"},
   {pM, cupg_hasupgr1,   s"MUpgr1",      s"CBIUpgr1"  },
   {pM, cupg_hasupgr2,   s"MUpgr2",      s"CBIUpgr2"  },
   {pM, cupg_rdistinter, s"MRDistInter"               },

   {pC, cupg_c_slot3spell, s"CSlot3Spell", s"Feuer"   },
   {pC, cupg_c_slot4spell, s"CSlot4Spell", s"Rend"    },
   {pC, cupg_c_slot5spell, s"CSlot5Spell", s"Hulgyon" },
   {pC, cupg_c_slot6spell, s"CSlot6Spell", s"StarShot"},
   {pC, cupg_c_slot7spell, s"CSlot7Spell", s"Cercle"  },
   {pC, cupg_c_rdistinter, s"CRDistInter"             },
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
void Lith_InstallCBIItem(i32 num)
{
   if(num < 0 || num >= cupg_max || world.cbiupgr[num]) return;

   world.cbiupgr[num] = true;

   switch(num) {
   case cupg_hasupgr1: world.cbiperf += 20; break;
   case cupg_hasupgr2: world.cbiperf += 40; break;
   }

   Lith_ForPlayer()
   {
      p->setActivator();

      ifauto(struct cupgdef const *, c, GetCUpgr(p->pclass, num))
         if(c->nam)
      {
         bip_name_t tag; lstrcpy_str(tag, c->nam);
         p->bipUnlock(tag);
      }
   }
}

void Lith_InstallSpawnedCBIItems(void)
{
   for(i32 i = 0; i < cbispawniter; i++)
      Lith_InstallCBIItem(cbispawn[i]);
}

script ext("ACS")
void Lith_CBIItemWasSpawned(i32 num)
{
   cbispawn[cbispawniter++] = num;
}

script ext("ACS")
void Lith_PickupCBIItem(i32 num)
{
   withplayer(LocalPlayer)
      Lith_FadeFlash(0, 255, 0, 0.7, 0.5);

   Lith_ForPlayer() {
      ifauto(struct cupgdef const *, c, GetCUpgr(p->pclass, num))
         if(c->msg) p->logB(1, LC(LANG "LOG_CBI"), Language(LANG "LOG_CBI_%S", c->msg));
   }

   Lith_InstallCBIItem(num);
}
#endif

// EOF
