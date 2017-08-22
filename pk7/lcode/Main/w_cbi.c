#include "lith_common.h"
#include "lith_world.h"
#include "lith_player.h"

static int lmvar cbispawn[cupg_max];
static int lmvar cbispawniter;

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
      if(num == cupg_c_slot3spell) p->bipUnlock("Feuer");
      if(num == cupg_c_slot4spell) p->bipUnlock("Rend");
      if(num == cupg_c_slot5spell) p->bipUnlock("Hulgyon");
      if(num == cupg_c_slot6spell) p->bipUnlock("StarShot");
      if(num == cupg_c_slot7spell) p->bipUnlock("Cercle");
      if(num == cupg_weapninter) p->bipUnlock("WeapnInter");
      if(num == cupg_weapninte2) p->bipUnlock("WeapnInte2");
      if(num == cupg_armorinter) p->bipUnlock("ArmorInter");
      if(num == cupg_hasupgr1  ) p->bipUnlock("CBIUpgr1");
      if(num == cupg_hasupgr2  ) p->bipUnlock("CBIUpgr2");
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
   __str mnam[] = {
      [cupg_hasupgr1  ] = "Upgr1",
      [cupg_hasupgr2  ] = "Upgr2",
      [cupg_armorinter] = "ArmorInter",
      [cupg_weapninter] = "WeapnInter",
      [cupg_weapninte2] = "WeapnInte2",
      [cupg_rdistinter] = "RDistInter",
   };

   __str cnam[] = {
      [cupg_c_slot3spell] = "Slot3Spell",
      [cupg_c_slot4spell] = "Slot4Spell",
      [cupg_c_slot5spell] = "Slot5Spell",
      [cupg_c_slot6spell] = "Slot6Spell",
      [cupg_c_slot7spell] = "Slot7Spell",
      [cupg_c_rdistinter] = "RDistInter",
   };

   Lith_ForPlayer()
   {
      switch(p->pclass)
      {
      case pcl_marine:
         p->log(Language("LITH_TXT_LOG_CBI_M%S", mnam[num]));
         break;
      case pcl_cybermage:
         p->log(Language("LITH_TXT_LOG_CBI_C%S", cnam[num]));
         break;
      }
   }

   Lith_InstallCBIItem(num);
}

// EOF
