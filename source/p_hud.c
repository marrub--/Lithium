/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Generalized HUD functions.
 *
 * ---------------------------------------------------------------------------|
 */

#include "p_player.h"
#include "p_hud.h"
#include "p_hudid.h"

/* Extern Functions -------------------------------------------------------- */

void HUD_WeaponSlots(i32 cr_one, i32 cr_two, i32 cr_many, i32 cr_cur, i32 _x, i32 _y) {
   if(CVarGetI(sc_hud_showweapons))
      for(i32 i = 1; i < SLOT_MAX; i++)
         ifauto(i32, slot, pl.weapon.slot[i])
   {
      k32 x = _x+0.2 - 8 * (SLOT_MAX - i);
      k32 y = _y+0.2;
      i32 cr;
      switch(slot) {
      case 1:  cr = cr_one;  break;
      case 2:  cr = cr_two;  break;
      default: cr = cr_many; break;
      }

      ACS_BeginPrint();
      ACS_PrintInt(i);
      PrintTextX(sf_lsmlhfnt, cr, x,2, y,2, _u_no_unicode);

      if(pl.weapon.cur->info->slot == i)
         SetFade(fid_slotnS + i, 1, 6);

      if(CheckFade(fid_slotnS + i)) {
         ACS_BeginPrint();
         ACS_PrintInt(i);
         PrintTextFX(sf_lsmlhfnt, cr_cur, x,2, y,2, fid_slotnS + i, _u_no_unicode);
      }
   }
}

void HUD_Score(cstr fmt, i96 scrn, str font, i32 cr, i32 x, i32 xa) {
   if(CVarGetI(sc_hud_showscore)) {
      char scr[64];
      sprintf(scr, fmt, scoresep(scrn));

      PrintTextChS(scr);
      PrintTextX(font, cr, x,xa, 3,1, _u_no_unicode);

      if(pl.score > pl.old.score) {
         SetFade(fid_schit1, 4, 12);
      } else if(pl.score < pl.old.score) {
         i96 scrdif = pl.old.score - pl.score;
         u32 tics = scrdif * 8 / 0xFFFF;
         u32 mmx = 30000 - minmax(scrdif / 4, 3276, 29490);
         SetFade(fid_schit2, minmax(tics, 1, 35), mmx >> 8);
      }

      if(pl.scoreaccumtime > 0) {
         SetFade(fid_scacum, 5, 12);
         pl.scoreaccumstr = StrParam("%c%s", pl.scoreaccum >= 0 ? '+' : ' ', scoresep(pl.scoreaccum));
      }

      if(CheckFade(fid_schit1)) {
         PrintTextChS(scr);
         PrintTextFX(font, CR_ORANGE, x,xa, 3,1, fid_schit1, _u_no_unicode);
      } else if(CheckFade(fid_schit2)) {
         PrintTextChS(scr);
         PrintTextFX(font, CR_PURPLE, x,xa, 3,1, fid_schit2, _u_no_unicode);
      }

      if(CheckFade(fid_scacum))
         PrintTextFX_str(pl.scoreaccumstr, font, CR_WHITE, x,xa, 10,1, fid_scacum, _u_no_unicode);
   }

   if(CVarGetI(sc_hud_showlvl)) {
      PrintTextFmt("Lv.%u", pl.attr.level);
      if(pl.attr.points) __nprintf(" (\Cn%u\C- pts)", pl.attr.points);
      PrintTextX(font, cr, x,xa, 17,1, _u_no_unicode);
   }

   i32 expbar = CVarGetI(sc_hud_expbar);
   if(expbar > 0) {
      Str(sp_exp_bar_0, s":Bars:ExpBar0");
      PrintSprite(sp_exp_bar_0, x,xa, 24,1);
      i32 xx = x;
      u32 fr =
         ((u64)(pl.attr.exp     - pl.attr.expprev) * 24) /
          (u64)(pl.attr.expnext - pl.attr.expprev);
      switch(xa) {
         case 0: xx -= 12; break;
         case 2: xx -= 24; break;
      }
      SetClip(xx, 24, fr, 2);
      PrintSprite(StrParam(":Bars:ExpBar%i", expbar), xx,1, 24,1);
      ClearClip();
   }
}

void HUD_KeyInd(i32 x, i32 y, bool horz, k32 a) {
   #define Inc (horz ? (x -= 10) : (y += 10))
   if(pl.krs) PrintSpriteA(sp_HUD_H_KS1, x,2, y,1, a), Inc;
   if(pl.kys) PrintSpriteA(sp_HUD_H_KS2, x,2, y,1, a), Inc;
   if(pl.kbs) PrintSpriteA(sp_HUD_H_KS3, x,2, y,1, a), Inc;
   if(pl.krc) PrintSpriteA(sp_HUD_H_KC1, x,2, y,1, a), Inc;
   if(pl.kyc) PrintSpriteA(sp_HUD_H_KC2, x,2, y,1, a), Inc;
   if(pl.kbc) PrintSpriteA(sp_HUD_H_KC3, x,2, y,1, a), Inc;
   #undef Inc
}

void HUD_DrawHealth(i32 health, i32 x, i32 y, i32 cr, i32 cr_fade) {
   i32 protect = pl.megaProtect;

   ACS_BeginPrint();
   ACS_PrintInt(health);
   PrintTextX(sf_bigupper, cr, x,1, y,0, _u_no_unicode);

   if(protect) {
      k32 amt = protect / 200.0;
      ACS_BeginPrint();
      ACS_PrintInt(health);
      PrintTextAX(sf_bigupper, CR_TAN, x,1, y,0, amt, _u_no_unicode);
   }

   if(CheckFade(fid_health)) {
      ACS_BeginPrint();
      ACS_PrintInt(health);
      PrintTextFX(sf_bigupper, cr_fade, x,1, y,0, fid_health, _u_no_unicode);
   }
}

/* EOF */
