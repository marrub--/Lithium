/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * HeadsUpDisC upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"
#include "p_hud.h"

Str(sp_HUD_C_AMMO,       s":HUD_C:AMMO");
Str(sp_HUD_C_Back,       s":HUD_C:Back");
Str(sp_HUD_C_Bar,        s":HUD_C:Bar");
Str(sp_HUD_C_HeatBar,    s":HUD_C:HeatBar");
Str(sp_HUD_C_MAG,        s":HUD_C:MAG");
Str(sp_HUD_C_SplitBack,  s":HUD_C:SplitBack");
Str(sp_HUD_C_SplitFront, s":HUD_C:SplitFront");
Str(sp_HUD_C_VIT,        s":HUD_C:VIT");

/* Static Functions -------------------------------------------------------- */

static void HUD_Ammo()
{
   struct invweapon const *wep = pl.weapon.cur;

   str typegfx = snil;

   if(wep->ammotype & AT_AMag) PrintSprite(sp_HUD_C_SplitFront, 320,2, 238,2);

   if(wep->ammotype & AT_NMag) {
      typegfx = sp_HUD_C_MAG;

      char txt[16];
      if(wep->ammotype & AT_Ammo && !wep->ammocur)
         strcpy(txt, "OUT");
      else
         sprintf(txt, "%i/%i", wep->magmax - wep->magcur, wep->magmax);

      PrintTextChS(txt);
      PrintTextX(sf_bigupper, Cr(red), 242,1, 229,0, _u_no_unicode);
   }

   if(wep->ammotype & AT_Ammo) {
      typegfx = sp_HUD_C_AMMO;

      i32 x = 0;

      if(wep->ammotype & AT_NMag) {
         PrintSprite(sp_HUD_C_Back, 240,2, 238,2);
         x = -58;
      }

      ACS_BeginPrint();
      ACS_PrintInt(wep->ammocur);
      PrintTextX(sf_bigupper, Cr(red), x+242,1, 229,0, _u_no_unicode);
   }

   if(typegfx)
      PrintSprite(typegfx, 316,2, 235,2);

   if(P_Wep_CurType() == weapon_c_smg) {
      i32 y = CVarGetI(sc_hud_showweapons) ? 0 : 14;
      k32 heat = InvNum(so_SMGHeat)/500.0k;
      PrintSprite(sp_HUD_C_BarSmall, 257,1, 196+y,1);
      SetClip(257, 196+y, 63 * heat, 9);
      PrintSprite(sp_HUD_C_HeatBar, 257,1, 196+y,1);
      ClearClip();
   }
}

static void HUD_Health()
{
   PrintSprite(sp_HUD_C_SplitBack, 0,1, 239,2);
   PrintSprite(sp_HUD_C_VIT,       2,1, 237,2);

   ACS_BeginPrint();
   ACS_PrintInt(pl.health);
   PrintTextX(sf_bigupper, Cr(red), 21,1, 229,0, _u_no_unicode);
   HUD_DrawHealth(pl.health, 21, 229, Cr(red), 0);
}

/* Extern Functions -------------------------------------------------------- */

void Upgr_HeadsUpDisC_Render(struct upgrade *upgr)
{
   if(!pl.hudenabled) return;

   HUD_Log(Cr(red), 0, -10);
   HUD_KeyInd(180, 21, true, 0.8);
   HUD_Score("%s\Cnscr", pl.score, sf_smallfnt, Cr(red), 160,0);

   if(CVarGetI(sc_hud_showweapons))
      PrintSprite(sp_HUD_C_Bar, 320,2, 220,2);

   HUD_WeaponSlots(Cr(wselc1), Cr(wselc2), Cr(wselc3), Cr(wselcs), 323, 219);

   /* Status */
   HUD_Ammo();
   HUD_Health();
}

/* EOF */
