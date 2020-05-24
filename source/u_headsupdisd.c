/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * HeadsUpDisD upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"
#include "p_hud.h"

Str(sp_HUD_D_Ammo2Back, s":HUD_D:Ammo2Back");
Str(sp_HUD_D_AmmoBack,  s":HUD_D:AmmoBack");
Str(sp_HUD_D_HPBack,    s":HUD_D:HPBack");
Str(sp_HUD_D_WepBack,   s":HUD_D:WepBack");

/* Static Functions -------------------------------------------------------- */

static void HUD_Ammo(struct player *p) {
   struct invweapon const *wep = p->weapon.cur;

   cstr type = nil;

   if(wep->ammotype & AT_AMag)
      PrintSprite(sp_HUD_D_AmmoBack, 320,2, 239,2);

   if(wep->ammotype & AT_NMag) {
      type = u8"\n";

      char txt[16];
      if(wep->ammotype & AT_Ammo && !wep->ammocur)
         strcpy(txt, "OUT");
      else
         sprintf(txt, "%i/%i", wep->magmax - wep->magcur, wep->magmax);
      PrintTextChS(txt);
      PrintTextX(s_bigupper, Cr(blue), 232,1, 228,0, ptf_no_utf);
   }

   if(wep->ammotype & AT_Ammo) {
      type = u8"\n";

      i32 x = 0;

      if(wep->ammotype & AT_NMag) {
         PrintSprite(sp_HUD_D_Ammo2Back, 230,2, 239,2);
         x = 62;
      }

      ACS_BeginPrint();
      ACS_PrintInt(wep->ammocur);
      PrintTextX(s_bigupper, Cr(blue), 232-x,1, 228,0, ptf_no_utf);
   }

   if(type) {
      PrintTextChS(type);
      PrintTextX(s_lmidfont, Cr(blue), 309,4, 230,0, ptf_no_utf);
   }
}

static void HUD_Health(struct player *p) {
   StrAry(ws, s":HUD:H_D27", s":HUD:H_D28", s":HUD:H_D24", s":HUD:H_D23",
              s":HUD:H_D22", s":HUD:H_D21", s":HUD:H_D25", s":HUD:H_D26");

   PrintSprite(sp_HUD_D_HPBack, 0,1, 239,2);

   PrintTextChS(u8"");
   PrintTextX(s_lmidfont, Cr(blue), 8,4, 229,0, ptf_no_utf);

   HUD_DrawHealth(p, p->health, 18, 228, Cr(blue), 0);

   str gfx = ws[p->weapon.cur->info->slot];

   i32 x = (8 + p->ticks) % 40;

   for(i32 i = 0; i < 20; i++) {
      i32 xx = x - i;
      if(xx < 0) xx += 40;

      PrintSpriteA(gfx, 21+xx,1, 220,1, (20 - i) / 20.0);
   }
}

/* Extern Functions -------------------------------------------------------- */

void Upgr_HeadsUpDisD_Render(struct player *p, struct upgrade *upgr) {
   if(!p->hudenabled) return;

   HUD_Log(p, Cr(blue), 0, -10);
   HUD_KeyInd(p, 180, 21, true, 0.8);
   HUD_Score(p, "%s\Cnscr", p->score, s_smallfnt, Cr(blue), 160,0);

   if(p->getCVarI(sc_hud_showweapons))
      PrintSprite(sp_HUD_D_WepBack, 320,2, 218,2);

   HUD_WeaponSlots(p, Cr(wseld1), Cr(wseld2), Cr(wseld3), Cr(wselds), 323, 216);

   /* Status */
   HUD_Ammo(p);
   HUD_Health(p);
}

/* EOF */
