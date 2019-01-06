// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// p_hud.c: Generalized HUD functions.

#include "p_player.h"
#include "p_hud.h"
#include "p_hudid.h"

// Extern Functions ----------------------------------------------------------|

void Lith_HUD_WeaponSlots_Impl(struct player *p, struct hud_wsl const *a)
{
   if(p->getCVarI(sc_hud_showweapons))
      for(i32 i = 1; i < SLOT_MAX; i++)
         ifauto(i32, slot, p->weapon.slot[i])
   {
      k32 x = a->x+.2 - 8 * (SLOT_MAX - i);
      k32 y = a->y+.2;

      PrintTextFmt("%i", i);
      PrintTextX(s_lhudfontsmall, a->ncol[min(slot - 1, 2)], x,2, y,2);

      if(p->weapon.cur->info->slot == i)
         SetFade(fid_slotnS + i, 1, 0.05);

      if(CheckFade(fid_slotnS + i))
      {
         PrintTextFmt("\C%S%i", a->scol, i);
         PrintTextFX(s_lhudfontsmall, 0, x,2, y,2, fid_slotnS + i);
      }
   }
}

void Lith_HUD_Score(struct player *p, char const *fmt, i96 scrn, str font, str cr, i32 x, i32 xa, i32 y, i32 ya)
{
   if(p->getCVarI(sc_hud_showscore))
   {
      str scr = StrParam(fmt, scoresep(scrn));

      PrintTextFmt("\C%S%S", cr, scr);
      PrintTextX(font, 0, x,xa, y,ya);

      if(p->score > p->old.score)
         SetFade(fid_schit1, 4, 0.1);
      else if(p->score < p->old.score)
         SetFade(fid_schit2, 4, 1 - minmax((p->old.score - p->score) / 30000.0, 0.1, 0.9));

      if(p->scoreaccumtime > 0)
      {
         SetFade(fid_scacum, 5, 0.1);
         p->scoreaccumstr = StrParam("%c%s", p->scoreaccum >= 0 ? '+' : ' ', scoresep(p->scoreaccum));
      }

      if(CheckFade(fid_schit1))
         PrintTextFX_str(scr, font, CR_ORANGE, x,xa, y,ya, fid_schit1);
      else if(CheckFade(fid_schit2))
         PrintTextFX_str(scr, font, CR_PURPLE, x,xa, y,ya, fid_schit2);

      if(CheckFade(fid_scacum))
         PrintTextFX_str(p->scoreaccumstr, font, CR_WHITE, x,xa, y+10,ya, fid_scacum);
   }

   if(p->getCVarI(sc_hud_showlvl))
   {
      PrintTextFmt("\C%SLv.%u", cr, p->attr.level);
      if(p->attr.points) __nprintf(" (%u pts)", p->attr.points);
      PrintTextX(font, 0, x,xa, y+20,ya);
   }
}

void Lith_HUD_KeyInd(struct player *p, i32 x, i32 y, bool horz, k32 a)
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

// EOF
