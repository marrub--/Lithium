/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * HeadsUpDisA upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"
#include "p_hud.h"

Str(sp_HUD_A_RightBack, s":HUD_A:RightBack");
Str(sp_HUD_A_LeftBack,  s":HUD_A:LeftBack");

/* Static Functions -------------------------------------------------------- */

static void HUD_Ammo() {
   struct invweapon const *wep = pl.weapon.cur;

   if(wep->ammotype & AT_AMag)
      PrintSprite(sp_HUD_A_RightBack, 320,2, 239,2);

   if(wep->ammotype & AT_NMag) {
      char txt[16];
      if(wep->ammotype & AT_Ammo && !wep->ammocur)
         strcpy(txt, "OUT");
      else
         sprintf(txt, "%i/%i", wep->magmax - wep->magcur, wep->magmax);
      PrintTextChS(txt);
      PrintTextX(sf_lmidfont, Cr(pink), 268,1, 225,1, ptf_no_utf);
   }

   if(wep->ammotype & AT_Ammo) {
      ACS_BeginPrint();
      ACS_PrintInt(wep->ammocur);
      PrintTextX(sf_lmidfont, Cr(pink), 270,1, 232,1, ptf_no_utf);
   }
}

/* Extern Functions -------------------------------------------------------- */

void Upgr_HeadsUpDisA_Render(struct upgrade *upgr) {
   if(!pl.hudenabled) return;

   HUD_Log(Cr(pink), 0, -5);
   HUD_KeyInd(320, 28, true, 0.8);
   HUD_Score("%s \CjSCR", pl.score, sf_lmidfont, Cr(pink), 320,2);

   HUD_WeaponSlots(Cr(wsela1), Cr(wsela2), Cr(wsela3), Cr(wselas), 323, 220);

   /* Status */
   HUD_Ammo();
   PrintSprite(sp_HUD_A_LeftBack, 0,1, 239,2);
   HUD_DrawHealth(pl.health, 4, 231, Cr(pink), 0);
}

/* EOF */
