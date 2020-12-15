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

#define UData p->upgrdata.headsupdisi

Str(sp_HUD_I_AMMO,         s":HUD_I:AMMO");
Str(sp_HUD_I_AmmoExtend,   s":HUD_I:AmmoExtend");
Str(sp_HUD_I_AmmoWepsBack, s":HUD_I:AmmoWepsBack");
Str(sp_HUD_I_HPAPBack,     s":HUD_I:HPAPBack");
Str(sp_HUD_I_MAG,          s":HUD_I:MAG");

/* Static Functions -------------------------------------------------------- */

static void HUDI_Ammo(struct player *p, struct upgrade *upgr) {
   struct invweapon const *wep = p->weapon.cur;

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
      PrintTextX(sf_bigupper, Cr(purple), x+242,1, 218,0, ptf_no_utf);
   }

   if(wep->ammotype & AT_NMag) {
      typegfx = sp_HUD_I_MAG;

      char txt[16];
      if(wep->ammotype & AT_Ammo && !wep->ammocur)
         strcpy(txt, "OUT");
      else
         sprintf(txt, "%i/%i", wep->magmax - wep->magcur, wep->magmax);
      PrintTextChS(txt);
      PrintTextX(sf_bigupper, Cr(purple), 242,1, 218,0, ptf_no_utf);
   }

   if(typegfx) {
      PrintSprite(typegfx, 309,0, 219,0);
   }

   HUD_WeaponSlots(p, Cr(wseli1), Cr(wseli2), Cr(wseli3), Cr(wselis), 323, 208);
}

static void HUDI_HealthArmor(struct player *p, struct upgrade *upgr)
{
   PrintSprite(sp_HUD_I_HPAPBack, 0,1, 230,2);

   UData.health.value = p->health;
   lerplli(&UData.health);

   HUD_DrawHealth(p, UData.health.value_display, 21, 202, Cr(purple), 0);

   UData.overdrive.value = p->overdrive;
   lerplli(&UData.overdrive);

   PrintTextFmt("%lli", UData.overdrive.value_display);
   PrintTextX(sf_bigupper, Cr(purple), 21,1, 220,0, ptf_no_utf);
}

/* Extern Functions -------------------------------------------------------- */

void Upgr_HeadsUpDisI_Activate(struct player *p, struct upgrade *upgr)
{
   lerplli_init(&UData.score,     p->score,     4);
   lerplli_init(&UData.health,    p->health,    1);
   lerplli_init(&UData.overdrive, p->overdrive, 1);
}

void Upgr_HeadsUpDisI_Render(struct player *p, struct upgrade *upgr)
{
   if(!p->hudenabled) return;

   HUD_Log(p, Cr(purple), 0, -15);

   HUD_KeyInd(p, 20, 20, false, 0.8);

   UData.score.value = p->score;
   lerplli(&UData.score);

   HUD_Score(p, "%s \CnScore", UData.score.value_display, sf_lmidfont, Cr(purple), 2,1);

   /* Status */
   HUDI_Ammo(p, upgr);
   HUDI_HealthArmor(p, upgr);
}

/* EOF */
