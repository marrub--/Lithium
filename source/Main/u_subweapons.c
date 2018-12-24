// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

StrEntON

// Extern Functions ----------------------------------------------------------|

script
void Upgr_Subweapons_Update(struct player *p, upgrade_t *upgr)
{
   if(InvNum(OBJ "SubweaponShots") < 2)
   {
      if(InvNum(OBJ "SubweaponCharge") == InvMax(OBJ "SubweaponCharge"))
      {
         ACS_LocalAmbientSound("weapons/subweapon/charge", 127);
         InvGive(OBJ "SubweaponShots", 1);
         InvTake(OBJ "SubweaponCharge", 999);
      }
      else
         InvGive(OBJ "SubweaponCharge", 1);
   }
}

stkcall
void Upgr_Subweapons_Render(struct player *p, upgrade_t *upgr)
{
   if(!p->hudenabled) return;

   int const sn = InvNum(OBJ "SubweaponShots");
   int const cn = InvNum(OBJ "SubweaponCharge");
   int const cm = InvMax(OBJ "SubweaponCharge");

   PrintTextFmt(c"SHOTS: %i\nCHARGE: %i/%i", sn, cn, cm);
   PrintText("chfont", CR_WHITE, 10,1, 120,2);
}

// EOF
