// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_player.h"

// Extern Functions ----------------------------------------------------------|

//
// Lith_HUD_Begin
//
void Lith_HUD_Begin(struct hud *h)
{
   h->id = h->beg;
}

//
// Lith_HUD_Clear
//
void Lith_HUD_Clear(struct hud *h)
{
   for(int i = h->beg; i <= h->id; i++)
   {
      ACS_BeginPrint();
      ACS_MoreHudMessage();
      ACS_OptHudMessage(0, i, 0, 0, 0, TS);
      ACS_EndHudMessage();
   }
}

//
// Lith_HUD_WeaponSlots_Impl
//
void Lith_HUD_WeaponSlots_Impl(struct hud *h, struct hud_wsl const *a)
{
   player_t *p = h->p;

   HID(wcurE, 9);
   HID(wtxtE, 9);

   ACS_SetFont("LHUDFONTSMALL");

   if(p->getCVarI("lith_hud_showweapons"))
      for(int i = 1; i < SLOT_MAX; i++)
         ifauto(int, slot, p->weapon.slot[i])
   {
      fixed x = a->x+.2 - 8 * (SLOT_MAX - i);
      fixed y = a->y+.2;

      HudMessage("%i", i);
      HudMessageParams(0, wtxtE + i, a->ncol[min(slot - 1, 2)], x, y, TS);

      if(p->weapon.cur->info->slot == i)
      {
         HudMessage("\C%S%i", a->scol, i);
         HudMessageFade(wcurE + i, x, y, TS, 0.2);
      }
   }
}

//
// Lith_HUD_Score_Impl
//
void Lith_HUD_Score_Impl(struct hud *h, __str scr, struct hud_scr const *a)
{
   player_t *p = h->p;

   ACS_SetFont(a->font);

   HID(scacu, 1);
   HID(schit, 1);
   HID(score, 1);

   if(p->getCVarI("lith_hud_showscore"))
   {
      fixed x = a->x;
      fixed y = a->y;

      HudMessage("\C%S%S", a->cr, scr);
      HudMessageParams(0, score, CR_WHITE, x, y, TS);

      if(p->score > p->old.score)
      {
         HudMessage("%S", scr);
         HudMessageParams(HUDMSG_FADEOUT, schit, CR_ORANGE, x, y, 0.1, 0.2);
      }
      else if(p->score < p->old.score)
      {
         fixed ft = minmax((p->old.score - p->score) / 3000.0, 0.1, 3.0);
         HudMessage("%S", scr);
         HudMessageParams(HUDMSG_FADEOUT, schit, CR_PURPLE, x, y, 0.1, ft);
      }

      if(p->scoreaccumtime > 0)
      {
         HudMessage("%c%S", p->scoreaccum >= 0 ? '+' : ' ',
            Lith_ScoreSep(p->scoreaccum));
         HudMessageParams(HUDMSG_FADEOUT, scacu, CR_WHITE, x, y + 10, 0.1, 0.4);
      }
   }
}

//
// Lith_HUD_KeyInd_Impl
//
void Lith_HUD_KeyInd_Impl(struct hud *h, struct hud_key const *a)
{
   player_t *p = h->p;

   int   x = a->x;
   int   y = a->y;
   fixed t = a->a;

   #define Inc ((a->horz) ? (x -= 10) : (y += 10))
   if(p->keys.rs) PrintSpriteA("H_KS1", x,2, y,1, t), Inc;
   if(p->keys.ys) PrintSpriteA("H_KS2", x,2, y,1, t), Inc;
   if(p->keys.bs) PrintSpriteA("H_KS3", x,2, y,1, t), Inc;
   if(p->keys.rc) PrintSpriteA("H_KC1", x,2, y,1, t), Inc;
   if(p->keys.yc) PrintSpriteA("H_KC2", x,2, y,1, t), Inc;
   if(p->keys.bc) PrintSpriteA("H_KC3", x,2, y,1, t), Inc;
   #undef Inc
}

//
// Lith_HUD_Log_Impl
//
[[__call("ScriptS")]]
void Lith_HUD_Log_Impl(struct hud *h, struct hud_log const *a)
{
   player_t *p = h->p;

   HID(logAddE, 20);
   HID(logE,    20);

   if(p->getCVarI("lith_hud_showlog"))
   {
      ACS_SetHudSize(480, 300);
      ACS_SetFont("LOGFONT");

      int i = 0;
      forlistIt(logdata_t *ld, p->loginfo.hud, i++)
      {
         int y = 10 * i;
         fixed align;

         if(p->getCVarI("lith_hud_logfromtop")) {
            y = 20 + y;
            align = 0.1;
         } else {
            y = (255 - y) + a->y;
            align = 0.2;
         }

         HudMessage("%S", ld->info);
         HudMessageParams(HUDMSG_NOWRAP, logE + i, a->cr, a->x+.1, y+align, TS);

         if(ld->time > LOG_TIME - 10)
         {
            HudMessage("%S", ld->info);
            HudMessageParams(HUDMSG_NOWRAP|HUDMSG_FADEOUT|HUDMSG_ADDBLEND, logAddE + i, a->cr, a->x+.1, y+align, TS, 0.15);
         }
      }
   }
}

// EOF
