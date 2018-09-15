// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_player.h"
#include "lith_hud.h"
#include "lith_hudid.h"

// Extern Functions ----------------------------------------------------------|

//
// Lith_HUD_WeaponSlots_Impl
//
void Lith_HUD_WeaponSlots_Impl(struct player *p, struct hud_wsl const *a)
{
   if(p->getCVarI("lith_hud_showweapons"))
      for(int i = 1; i < SLOT_MAX; i++)
         ifauto(int, slot, p->weapon.slot[i])
   {
      fixed x = a->x+.2 - 8 * (SLOT_MAX - i);
      fixed y = a->y+.2;

      PrintTextFmt("%i", i);
      PrintText("LHUDFONTSMALL", a->ncol[min(slot - 1, 2)], x,2, y,2);

      if(p->weapon.cur->info->slot == i)
         SetFade(fid_slotnS + i, 1, 0.05);

      if(CheckFade(fid_slotnS + i))
      {
         PrintTextFmt("\C%S%i", a->scol, i);
         PrintTextF("LHUDFONTSMALL", 0, x,2, y,2, fid_slotnS + i);
      }
   }
}

//
// Lith_HUD_Score
//
void Lith_HUD_Score(struct player *p, __str fmt, i96 scrn, __str font, __str cr, int x, int xa, int y, int ya)
{
   if(p->getCVarI("lith_hud_showscore"))
   {
      __str scr = StrParam(fmt, Lith_ScoreSep(scrn));

      PrintTextFmt("\C%S%S", cr, scr);
      PrintText(font, 0, x,xa, y,ya);

      if(p->score > p->old.score)
         SetFade(fid_schit1, 4, 0.1);
      else if(p->score < p->old.score)
         SetFade(fid_schit2, 4, 1 - minmax((p->old.score - p->score) / 30000.0, 0.1, 0.9));

      if(p->scoreaccumtime > 0)
      {
         SetFade(fid_scacum, 5, 0.1);
         p->scoreaccumstr = StrParam("%c%S", p->scoreaccum >= 0 ? '+' : ' ', Lith_ScoreSep(p->scoreaccum));
      }

      if(CheckFade(fid_schit1))
      {
         PrintTextFmt("%S", scr);
         PrintTextF(font, CR_ORANGE, x,xa, y,ya, fid_schit1);
      }
      else if(CheckFade(fid_schit2))
      {
         PrintTextFmt("%S", scr);
         PrintTextF(font, CR_PURPLE, x,xa, y,ya, fid_schit2);
      }

      if(CheckFade(fid_scacum))
      {
         PrintTextStr(p->scoreaccumstr);
         PrintTextF(font, CR_WHITE, x,xa, y+10,ya, fid_scacum);
      }
   }
}

//
// Lith_HUD_KeyInd
//
void Lith_HUD_KeyInd(struct player *p, int x, int y, bool horz, fixed a)
{
   #define Inc (horz ? (x -= 10) : (y += 10))
   if(p->keys.rs) PrintSpriteA(":HUD:H_KS1", x,2, y,1, a), Inc;
   if(p->keys.ys) PrintSpriteA(":HUD:H_KS2", x,2, y,1, a), Inc;
   if(p->keys.bs) PrintSpriteA(":HUD:H_KS3", x,2, y,1, a), Inc;
   if(p->keys.rc) PrintSpriteA(":HUD:H_KC1", x,2, y,1, a), Inc;
   if(p->keys.yc) PrintSpriteA(":HUD:H_KC2", x,2, y,1, a), Inc;
   if(p->keys.bc) PrintSpriteA(":HUD:H_KC3", x,2, y,1, a), Inc;
   #undef Inc
}

//
// Lith_HUD_Log
//
script
void Lith_HUD_Log(struct player *p, int cr, int x, int yy)
{
   if(p->getCVarI("lith_hud_showlog"))
   {
      SetSize(480, 300);

      int i = 0;
      forlistIt(logdata_t *ld, p->loginfo.hud, i++)
      {
         int y = 10 * i;
         int ya;

         if(p->getCVarI("lith_hud_logfromtop"))
            {ya = 1; y = 20 + y;}
         else
            {ya = 2; y = (255 - y) + yy;}

         PrintTextStr(ld->info);
         PrintText("LOGFONT", cr, x,1, y,ya);

         if(ld->time > LOG_TIME - 10)
            SetFade(fid_logadS + i, 1, 0.07);

         if(CheckFade(fid_logadS + i))
         {
            PrintTextStr(ld->info);
            PrintTextF("LOGFONT", CR_WHITE, x,1, y,ya, fid_logadS + i);
         }
      }

      SetSize(320, 240);
   }
}

// EOF
