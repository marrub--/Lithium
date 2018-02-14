// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Extern Functions ----------------------------------------------------------|

//
// Update
//
[[__call("ScriptS")]]
void Upgr_Subweapons_Update(player_t *p, upgrade_t *upgr)
{
   if(InvNum("Lith_SubweaponShots") < 2)
   {
      if(InvNum("Lith_SubweaponCharge") == InvMax("Lith_SubweaponCharge"))
      {
         ACS_LocalAmbientSound("weapons/subweapon/charge", 127);
         InvGive("Lith_SubweaponShots", 1);
         InvTake("Lith_SubweaponCharge", 999);
      }
      else
         InvGive("Lith_SubweaponCharge", 1);
   }
}

//
// Render
//
void Upgr_Subweapons_Render(player_t *p, upgrade_t *upgr)
{
   if(!p->hudenabled) return;

   int const sn = InvNum("Lith_SubweaponShots");
   int const cn = InvNum("Lith_SubweaponCharge");
   int const cm = InvMax("Lith_SubweaponCharge");

   PrintTextFmt("SHOTS: %i\nCHARGE: %i/%i", sn, cn, cm);
   PrintText("CHFONT", CR_WHITE, 10,1, 120,2);
}

// EOF
