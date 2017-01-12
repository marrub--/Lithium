#include "lith_upgrades_common.h"


//----------------------------------------------------------------------------
// Static Functions
//

//
// RA_Take
//
static void RA_Take(void)
{
   ACS_TakeInventory("Lith_RA_Bullets",   999);
   ACS_TakeInventory("Lith_RA_Energy",    999);
   ACS_TakeInventory("Lith_RA_Fire",      999);
   ACS_TakeInventory("Lith_RA_FireMagic", 999);
   ACS_TakeInventory("Lith_RA_Magic",     999);
   ACS_TakeInventory("Lith_RA_Melee",     999);
   ACS_TakeInventory("Lith_RA_Shrapnel",  999);
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_RA_Give
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_RA_Give(int num)
{
   if(!Lith_LocalPlayer->upgrades[UPGR_ReactArmour].active) return;
   
   __str name;
   
   switch(num)
   {
   case 1: name = "Bullets";   break;
   case 2: name = "Energy";    break;
   case 3: name = "Fire";      break;
   case 4: name = "FireMagic"; break;
   case 5: name = "Magic";     break;
   case 6: name = "Melee";     break;
   case 7: name = "Shrapnel";  break;
   }
   
   __str class = StrParam("Lith_RA_%S", name);
   
   if(!ACS_CheckInventory(class))
   {
      RA_Take();
      Lith_Log(Lith_LocalPlayer, ">>>>> Activating Armor->%S()", name);
      ACS_GiveInventory(class, 1);
   }
}

//
// Deactivate
//
void Upgr_ReactArmour_Deactivate(player_t *p, upgrade_t *upgr)
{
   RA_Take();
}

// EOF

