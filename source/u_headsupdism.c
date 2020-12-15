/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * HeadsUpDisM upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"
#include "p_hud.h"

#define UData p->upgrdata.headsupdism

Str(sp_HUD_M_Bar,          s":HUD_M:Bar");
Str(sp_HUD_M_BarBig,       s":HUD_M:BarBig");
Str(sp_HUD_M_SplitBack,    s":HUD_M:SplitBack");
Str(sp_HUD_M_SplitBackRed, s":HUD_M:SplitBackRed");
Str(sp_HUD_M_SplitFront,   s":HUD_M:SplitFront");
Str(sp_HUD_M_SplitRight,   s":HUD_M:SplitRight");

/* Static Functions -------------------------------------------------------- */

static void HUD_Ammo(struct player *p) {
   struct invweapon const *wep = p->weapon.cur;

   str typebg;
   i32 y;

   if(p->getCVarI(sc_hud_showweapons)) {y = 13; typebg = sp_HUD_M_SplitRight;}
   else                                {y = 0;  typebg = sp_HUD_M_SplitFront;}

   if(wep->ammotype & AT_NMag || wep->ammotype & AT_Ammo) {
      PrintSprite(sp_HUD_M_BarBig, 278,2, 239-y,2);
      PrintSprite(typebg, 319,2, 239,2);
   }

   cstr type = nil;

   if(wep->ammotype & AT_NMag) {
      type = "MAG";

      char txt[16];
      if(wep->ammotype & AT_Ammo && !wep->ammocur)
         strcpy(txt, "OUT");
      else
         sprintf(txt, "%i/%i", wep->magmax - wep->magcur, wep->magmax);
      PrintTextChS(txt);
      PrintTextX(sf_bigupper, Cr(green), 217,1, 238-y,2, ptf_no_utf);
   }

   if(wep->ammotype & AT_Ammo) {
      type = "AMMO";

      i32 x = 0;

      if(wep->ammotype & AT_NMag) {
         PrintSprite(sp_HUD_M_BarBig, 214,2, 239-y,2);
         x = 64;
      }

      ACS_BeginPrint();
      ACS_PrintInt(wep->ammocur);
      PrintTextX(sf_bigupper, Cr(green), 217-x,1, 238-y,2, ptf_no_utf);
   }

   if(type) {
      PrintTextChS(type);
      PrintTextX(sf_bigupper, Cr(green), 281,1, 238,2, ptf_no_utf);
   }

   if(P_Wep_CurType(p) == weapon_rifle && ServCallI(sm_GetRifleGrenade))
      PrintSprite(sp_HUD_H_D44, 280,1, 237,1);
}

static void HUD_Health(struct player *p, struct upgrade *upgr) {
   StrAry(ws, s":HUD:H_D27", s":HUD:H_D28", s":HUD:H_D24", s":HUD:H_D23",
              s":HUD:H_D22", s":HUD:H_D21", s":HUD:H_D25", s":HUD:H_D26");

   PrintSprite(InvNum(so_PowerStrength) ? sp_HUD_M_SplitBackRed : sp_HUD_M_SplitBack, 0,1, 240,2);

   PrintTextChr("VIT", 3);
   PrintTextX(sf_bigupper, Cr(green), 2,1, 238,2, ptf_no_utf);

   k32 ft = 0;

   if(p->health < p->oldhealth) {
      UData.cr = CR_PURPLE;
      ft = (p->oldhealth - p->health) / 300.0;
      ft = minmax(ft, 0.1, 1.0);
   } else if(p->health > p->oldhealth) {
      UData.cr = CR_YELLOW;
      ft = 0.1;
   }

   if(ft) SetFade(fid_health, 4, ft * 255);

   HUD_DrawHealth(p, p->health, 34, 232, Cr(green), UData.cr);

   str gfx = ws[p->weapon.cur->info->slot];

   i32 x = (8 + p->ticks) % 57;

   for(i32 i = 0; i < 20; i++) {
      i32 xx = x - i;
      if(xx < 0) xx += 57;

      i32 y = 9;
      if(xx < 11) y += 11 - xx % 12;

      PrintSpriteA(gfx, 88-xx,1, 215+y,1, (20 - i) / 20.0);
   }
}

/* Extern Functions -------------------------------------------------------- */

void Upgr_HeadsUpDisM_Render(struct player *p, struct upgrade *upgr) {
   if(!p->hudenabled) return;

   HUD_Log(p, Cr(green), 0, 0);

   HUD_KeyInd(p, 320, 20, true, 0.8);
   HUD_Score(p, "%s\Cnscr", p->score, sf_smallfnt, CR_WHITE, 320,2);

   if(p->getCVarI(sc_hud_showweapons))
      PrintSprite(sp_HUD_M_Bar, 278,2, 239,2);

   HUD_WeaponSlots(p, Cr(wselm1), Cr(wselm2), Cr(wselm3), Cr(wselms), 281, 238);

   /* Status */
   HUD_Ammo(p);
   HUD_Health(p, upgr);
}

/* EOF */
