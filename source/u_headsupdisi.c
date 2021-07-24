/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * HeadsUpDisI upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"
#include "p_hud.h"

#define UData pl.upgrdata.headsupdisi

/* Static Functions -------------------------------------------------------- */

static
void HUDI_Ammo(struct upgrade *upgr) {
   struct invweapon const *wep = pl.weapon.cur;

   str typegfx = snil;

   if(wep->ammotype & AT_Ammo && wep->ammotype & AT_NMag) {
      PrintSprite(sp_HUD_I_AmmoExtend, 242,2, 227,2);
   }

   PrintSprite(sp_HUD_I_AmmoWepsBack, 320,2, 229,2);

   if(wep->ammotype & AT_Ammo) {
      typegfx = sp_HUD_I_AMMO;

      i32 x = 0;

      if(wep->ammotype & AT_NMag) {
         x = -58;
      }

      ACS_BeginPrint();
      ACS_PrintInt(wep->ammocur);
      PrintTextX(sf_bigupper, Cr(purple), x+242,1, 218,0, _u_no_unicode);
   }

   if(wep->ammotype & AT_NMag) {
      typegfx = sp_HUD_I_MAG;

      char txt[16];
      if(wep->ammotype & AT_Ammo && !wep->ammocur)
         faststrcpy(txt, "OUT");
      else
         sprintf(txt, "%i/%i", wep->magmax - wep->magcur, wep->magmax);
      PrintTextChS(txt);
      PrintTextX(sf_bigupper, Cr(purple), 242,1, 218,0, _u_no_unicode);
   }

   if(typegfx) {
      PrintSprite(typegfx, 309,0, 219,0);
   }

   HUD_WeaponSlots(Cr(wseli1), Cr(wseli2), Cr(wseli3), Cr(wselis), 323, 208);
}

static
void HUDI_HealthArmor(struct upgrade *upgr)
{
   PrintSprite(sp_HUD_I_HPAPBack, 0,1, 230,2);

   UData.health.value = pl.health;
   lerplli(&UData.health);

   HUD_DrawHealth(UData.health.value_display, 21, 202, Cr(purple), 0);

   UData.overdrive.value = pl.overdrive;
   lerplli(&UData.overdrive);

   PrintTextFmt("%lli", UData.overdrive.value_display);
   PrintTextX(sf_bigupper, Cr(purple), 21,1, 220,0, _u_no_unicode);
}

/* Extern Functions -------------------------------------------------------- */

void Upgr_HeadsUpDisI_Activate(struct upgrade *upgr)
{
   lerplli_init(&UData.score,     pl.score,     4);
   lerplli_init(&UData.health,    pl.health,    1);
   lerplli_init(&UData.overdrive, pl.overdrive, 1);
}

void Upgr_HeadsUpDisI_Render(struct upgrade *upgr)
{
   if(!pl.hudenabled) return;

   HUD_Log(Cr(purple), 0, -15);

   UData.score.value = pl.score;
   lerplli(&UData.score);

   HUD_Score("%s \CnScore", UData.score.value_display, sf_lmidfont, Cr(purple));

   /* Status */
   HUDI_Ammo(upgr);
   HUDI_HealthArmor(upgr);
}

/* EOF */
