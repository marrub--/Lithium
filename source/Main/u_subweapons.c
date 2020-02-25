/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Subweapons upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

/* Extern Functions -------------------------------------------------------- */

script
void Upgr_Subweapons_Update(struct player *p, struct upgrade *upgr)
{
   if(InvNum(so_SubweaponShots) < 2)
   {
      if(InvNum(so_SubweaponCharge) == InvMax(so_SubweaponCharge))
      {
         ACS_LocalAmbientSound(ss_weapons_subweapon_charge, 127);
         InvGive(so_SubweaponShots, 1);
         InvTake(so_SubweaponCharge, INT32_MAX);
      }
      else
         InvGive(so_SubweaponCharge, 1);
   }
}

stkcall
void Upgr_Subweapons_Render(struct player *p, struct upgrade *upgr)
{
   if(!p->hudenabled) return;

   i32 const sn = InvNum(so_SubweaponShots);
   i32 const cn = InvNum(so_SubweaponCharge);
   i32 const cm = InvMax(so_SubweaponCharge);

   i32 const prc = cn == 0 ? 100 : cn / (k32)cm * 100;

   PrintTextFmt("SHOTS: %i\nCHARGE: %-3i%%", sn, prc);
   PrintText(s_chfont, CR_WHITE, 10,1, 120,2);
}

/* EOF */
