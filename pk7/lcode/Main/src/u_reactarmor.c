#include "lith_upgrades_common.h"

#define UserData upgr->UserData_ReactArmor


//----------------------------------------------------------------------------
// Static Functions
//

//
// RA_Take
//
static void RA_Take(int n)
{
   ACS_TakeInventory(StrParam("Lith_RA_Bullets%i", n),   999);
   ACS_TakeInventory(StrParam("Lith_RA_Energy%i", n),    999);
   ACS_TakeInventory(StrParam("Lith_RA_Fire%i", n),      999);
   ACS_TakeInventory(StrParam("Lith_RA_FireMagic%i", n), 999);
   ACS_TakeInventory(StrParam("Lith_RA_Magic%i", n),     999);
   ACS_TakeInventory(StrParam("Lith_RA_Melee%i", n),     999);
   ACS_TakeInventory(StrParam("Lith_RA_Shrapnel%i", n),  999);
   ACS_TakeInventory(StrParam("Lith_RA_Ice%i", n),       999);
}

//
// RA_Give
//
static void RA_Give(__str name, int n)
{
   ACS_GiveInventory(StrParam("Lith_RA_%S%i", name, n), 1);
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
   player_t *p = LocalPlayer;
   upgrade_t *upgr = &p->upgrades[UPGR_ReactArmor];
   
   if(!upgr->active)
      return;
   
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
   case 8: name = "Ice";       break;
   }
   
   if(UserData.activearmor != num)
   {
      UserData.activearmor = num;
      
      RA_Take(1);
      RA_Take(2);
      
      p->logH(">>>>> Activating Armor->%S()", name);
      
      if(p->upgrades[UPGR_ReactArmor2].active)
         RA_Give(name, 2);
      else
         RA_Give(name, 1);
   }
}

//
// Deactivate
//
void Upgr_ReactArmor_Deactivate(player_t *p, upgrade_t *upgr)
{
   UserData.activearmor = 0;
   
   RA_Take(1);
   RA_Take(2);
}

// EOF

