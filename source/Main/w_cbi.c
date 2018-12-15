// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_world.h"
#include "lith_player.h"

StrEntON

static int lmvar cbispawn[cupg_max];
static int lmvar cbispawniter;

struct cupgdef
{
   int pclass;
   int key;
   __str msg;
   __str nam;
};

static struct cupgdef const cdefs[] = {
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

struct cupgdef const *GetCUpgr(int pclass, int num)
{
   for(int i = 0; i < countof(cdefs); i++) {
      struct cupgdef const *c = &cdefs[i];
      if(c->pclass & pclass && c->key == num)
         return c;
   }
   return null;
}

script
void Lith_InstallCBIItem(int num)
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
   for(int i = 0; i < cbispawniter; i++)
      Lith_InstallCBIItem(cbispawn[i]);
}

script ext("ACS")
void Lith_CBIItemWasSpawned(int num)
{
   cbispawn[cbispawniter++] = num;
}

script ext("ACS")
void Lith_PickupCBIItem(int num)
{
   withplayer(LocalPlayer)
      Lith_FadeFlash(0, 255, 0, 0.7, 0.5);

   Lith_ForPlayer() {
      ifauto(struct cupgdef const *, c, GetCUpgr(p->pclass, num))
         if(c->msg) p->logB(1, L(LANG "LOG_CBI"), Language(LANG "LOG_CBI_%S", c->msg));
   }

   Lith_InstallCBIItem(num);
}

// EOF
