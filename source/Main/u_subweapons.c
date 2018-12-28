// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "u_common.h"

// Extern Functions ----------------------------------------------------------|

script
void Upgr_Subweapons_Update(struct player *p, upgrade_t *upgr)
{
   if(InvNum(so_SubweaponShots) < 2)
   {
      if(InvNum(so_SubweaponCharge) == InvMax(so_SubweaponCharge))
      {
         ACS_LocalAmbientSound(ss_weapons_subweapon_charge, 127);
         InvGive(so_SubweaponShots, 1);
         InvTake(so_SubweaponCharge, 999);
      }
      else
         InvGive(so_SubweaponCharge, 1);
   }
}

stkcall
void Upgr_Subweapons_Render(struct player *p, upgrade_t *upgr)
{
   if(!p->hudenabled) return;

   i32 const sn = InvNum(so_SubweaponShots);
   i32 const cn = InvNum(so_SubweaponCharge);
   i32 const cm = InvMax(so_SubweaponCharge);

   PrintTextFmt("SHOTS: %i\nCHARGE: %i/%i", sn, cn, cm);
   PrintText(s_chfont, CR_WHITE, 10,1, 120,2);
}

// EOF
