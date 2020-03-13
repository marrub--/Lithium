/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
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

void HUD_WeaponSlots_Impl(struct player *p, struct hud_wsl const *a)
{
   if(p->getCVarI(sc_hud_showweapons))
      for(i32 i = 1; i < SLOT_MAX; i++)
         ifauto(i32, slot, p->weapon.slot[i])
   {
      k32 x = a->x+.2 - 8 * (SLOT_MAX - i);
      k32 y = a->y+.2;

      PrintTextFmt("%i", i);
      PrintTextX(s_lhudfontsmall, a->ncol[min(slot - 1, 2)], x,2, y,2, ptf_no_utf);

      if(p->weapon.cur->info->slot == i)
         SetFade(fid_slotnS + i, 1, 6);

      if(CheckFade(fid_slotnS + i))
      {
         PrintTextFmt("\C%S%i", a->scol, i);
         PrintTextFX(s_lhudfontsmall, 0, x,2, y,2, fid_slotnS + i, ptf_no_utf);
      }
   }
}

void HUD_Score(struct player *p, cstr fmt, i96 scrn, str font, str cr, i32 x, i32 xa, i32 y, i32 ya)
{
   if(p->getCVarI(sc_hud_showscore))
   {
      str scr = StrParam(fmt, scoresep(scrn));

      PrintTextFmt("\C%S%S", cr, scr);
      PrintTextX(font, 0, x,xa, y,ya, ptf_no_utf);

      if(p->score > p->old.score) {
         SetFade(fid_schit1, 4, 12);
      } else if(p->score < p->old.score) {
         i96 scrdif = p->old.score - p->score;
         u32 tics = scrdif * 8 / 0xFFFF;
         u32 mmx = 30000 - minmax(scrdif / 4, 3276, 29490);
         SetFade(fid_schit2, minmax(tics, 1, 35), mmx >> 8);
      }

      if(p->scoreaccumtime > 0) {
         SetFade(fid_scacum, 5, 12);
         p->scoreaccumstr = StrParam("%c%s", p->scoreaccum >= 0 ? '+' : ' ', scoresep(p->scoreaccum));
      }

      if(CheckFade(fid_schit1))
         PrintTextFX_str(scr, font, CR_ORANGE, x,xa, y,ya, fid_schit1, ptf_no_utf);
      else if(CheckFade(fid_schit2))
         PrintTextFX_str(scr, font, CR_PURPLE, x,xa, y,ya, fid_schit2, ptf_no_utf);

      if(CheckFade(fid_scacum))
         PrintTextFX_str(p->scoreaccumstr, font, CR_WHITE, x,xa, y+10,ya, fid_scacum, ptf_no_utf);
   }

   if(p->getCVarI(sc_hud_showlvl))
   {
      PrintTextFmt("\C%SLv.%u", cr, p->attr.level);
      if(p->attr.points) __nprintf(" (%u pts)", p->attr.points);
      PrintTextX(font, 0, x,xa, y+20,ya, ptf_no_utf);
   }
}

void HUD_KeyInd(struct player *p, i32 x, i32 y, bool horz, k32 a)
{
   #define Inc (horz ? (x -= 10) : (y += 10))
   if(p->krs) PrintSpriteA(sp_HUD_H_KS1, x,2, y,1, a), Inc;
   if(p->kys) PrintSpriteA(sp_HUD_H_KS2, x,2, y,1, a), Inc;
   if(p->kbs) PrintSpriteA(sp_HUD_H_KS3, x,2, y,1, a), Inc;
   if(p->krc) PrintSpriteA(sp_HUD_H_KC1, x,2, y,1, a), Inc;
   if(p->kyc) PrintSpriteA(sp_HUD_H_KC2, x,2, y,1, a), Inc;
   if(p->kbc) PrintSpriteA(sp_HUD_H_KC3, x,2, y,1, a), Inc;
   #undef Inc
}

void HUD_DrawHealth(struct player *p, i32 health, i32 x, i32 y, char const *pfx, i32 cr)
{
   i32 protect = p->megaProtect;

   PrintTextFmt("%s%i", pfx, health);
   PrintTextX(s_lhudfont, 0, x,1, y,0, ptf_no_utf);

   if(protect) {
      k32 amt = protect / 200.0;
      PrintTextFmt("%i", health);
      PrintTextAX(s_lhudfont, CR_TAN, x,1, y,0, amt, ptf_no_utf);
   }

   if(CheckFade(fid_health)) {
      PrintTextFmt("%i", health);
      PrintTextFX(s_lhudfont, cr, x,1, y,0, fid_health, ptf_no_utf);
   }
}

/* EOF */
