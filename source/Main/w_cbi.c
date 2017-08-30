// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_world.h"
#include "lith_player.h"

static int lmvar cbispawn[cupg_max];
static int lmvar cbispawniter;

struct cupgdef
{
   int pclass;
   int key;
   __str msg;
   __str bipunlock;
};

static struct cupgdef const cdefs[] = {
   {pcl_marine, cupg_weapninter, "MWeapnInter", "WeapnInter"},
   {pcl_marine, cupg_weapninte2, "MWeapnInte2", "WeapnInte2"},
   {pcl_marine, cupg_armorinter, "MArmorInter", "ArmorInter"},
   {pcl_marine, cupg_hasupgr1,   "MUpgr1",      "CBIUpgr1"  },
   {pcl_marine, cupg_hasupgr2,   "MUpgr2",      "CBIUpgr2"  },
   {pcl_marine, cupg_rdistinter, "MRDistInter"              },

   {pcl_cybermage, cupg_c_slot3spell, "CSlot3Spell", "Feuer"   },
   {pcl_cybermage, cupg_c_slot4spell, "CSlot4Spell", "Rend"    },
   {pcl_cybermage, cupg_c_slot5spell, "CSlot5Spell", "Hulgyon" },
   {pcl_cybermage, cupg_c_slot6spell, "CSlot6Spell", "StarShot"},
   {pcl_cybermage, cupg_c_slot7spell, "CSlot7Spell", "Cercle"  },
   {pcl_cybermage, cupg_c_rdistinter, "CRDistInter"            },
};

//
// GetCUpgr
//
struct cupgdef const *GetCUpgr(int pclass, int num)
{
   for(int i = 0; i < countof(cdefs); i++) {
      struct cupgdef const *c = &cdefs[i];
      if(c->pclass & pclass && c->key == num)
         return c;
   }
}

//
// Lith_InstallCBIItem
//
void Lith_InstallCBIItem(int num)
{
   if(num < 0 || num >= cupg_max || world.cbiupgr[num])
      return;

   world.cbiupgr[num] = true;

   switch(num) {
   case cupg_hasupgr1: world.cbiperf += 20; break;
   case cupg_hasupgr2: world.cbiperf += 40; break;
   }

   Lith_ForPlayer() {
      ifauto(struct cupgdef const *, c, GetCUpgr(p->pclass, num))
         if(c->bipunlock) p->bipUnlock(c->bipunlock);
   }
}

//
// Lith_InstallSpawnedCBIItems
//
void Lith_InstallSpawnedCBIItems(void)
{
   for(int i = 0; i < cbispawniter; i++)
      Lith_InstallCBIItem(cbispawn[i]);
}

//
// Lith_CBIItemWasSpawned
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_CBIItemWasSpawned(int num)
{
   cbispawn[cbispawniter++] = num;
}

//
// Lith_PickupCBIItem
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_PickupCBIItem(int num)
{
   Lith_ForPlayer() {
      ifauto(struct cupgdef const *, c, GetCUpgr(p->pclass, num))
         if(c->msg) p->log("%S", Language("LITH_TXT_LOG_CBI_%S", c->msg));
   }

   Lith_InstallCBIItem(num);
}

// EOF
