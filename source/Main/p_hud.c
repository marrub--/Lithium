// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "lith_player.h"
#include "lith_hud.h"
#include "lith_hudid.h"

// Extern Functions ----------------------------------------------------------|

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

void Lith_HUD_Score(struct player *p, __str fmt, i96 scrn, __str font, __str cr, int x, int xa, int y, int ya)
{
   if(p->getCVarI("lith_hud_showscore"))
   {
      __str scr = StrParam(fmt, scoresep(scrn));

      PrintTextFmt("\C%S%S", cr, scr);
      PrintText(font, 0, x,xa, y,ya);

      if(p->score > p->old.score)
         SetFade(fid_schit1, 4, 0.1);
      else if(p->score < p->old.score)
         SetFade(fid_schit2, 4, 1 - minmax((p->old.score - p->score) / 30000.0, 0.1, 0.9));

      if(p->scoreaccumtime > 0)
      {
         SetFade(fid_scacum, 5, 0.1);
         p->scoreaccumstr = StrParam("%c%S", p->scoreaccum >= 0 ? '+' : ' ', scoresep(p->scoreaccum));
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

void Lith_HUD_KeyInd(struct player *p, int x, int y, bool horz, fixed a)
{
   #define Inc (horz ? (x -= 10) : (y += 10))
   if(p->krs) PrintSpriteA(":HUD:H_KS1", x,2, y,1, a), Inc;
   if(p->kys) PrintSpriteA(":HUD:H_KS2", x,2, y,1, a), Inc;
   if(p->kbs) PrintSpriteA(":HUD:H_KS3", x,2, y,1, a), Inc;
   if(p->krc) PrintSpriteA(":HUD:H_KC1", x,2, y,1, a), Inc;
   if(p->kyc) PrintSpriteA(":HUD:H_KC2", x,2, y,1, a), Inc;
   if(p->kbc) PrintSpriteA(":HUD:H_KC3", x,2, y,1, a), Inc;
   #undef Inc
}

script
void Lith_HUD_Log(struct player *p, int cr, int x, int yy)
{
   if(p->getCVarI("lith_hud_showlog"))
   {
      int yo;

      if(p->getCVarI("lith_hud_logbig")) {yo = 200; SetSize(320, 240);}
      else                               {yo = 255; SetSize(480, 300);}

      int i = 0;
      forlistIt(logdata_t *ld, p->loginfo.hud, i++)
      {
         int y = 10 * i;
         int ya;

         if(p->getCVarI("lith_hud_logfromtop"))
            {ya = 1; y = 20 + y;}
         else
            {ya = 2; y = (yo - y) + yy;}

         PrintTextStr(ld->info);
         PrintText("logfont", cr, x,1, y,ya);

         if(ld->time > LOG_TIME - 10)
            SetFade(fid_logadS + i, 1, 0.07);

         if(CheckFade(fid_logadS + i))
         {
            PrintTextStr(ld->info);
            PrintTextF("logfont", CR_WHITE, x,1, y,ya, fid_logadS + i);
         }
      }

      SetSize(320, 240);
   }
}

// EOF
