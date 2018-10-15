// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"
#include "lith_cps.h"

#include <ctype.h>

//
// Lith_GUI_TextBox_Impl
//
gui_txtbox_state_t *Lith_GUI_TextBox_Impl(gui_state_t *g, id_t id, gui_txtbox_args_t const *a)
{
   gui_txtbox_state_t *st = a->st;

   Lith_GUI_Auto(g, id, a->x, a->y, 260, 10);

   bool hot = g->hot == id;

   if(hot) a->p->grabInput = true;

   if(*a->p->txtbuf)
      ACS_LocalAmbientSound("player/cbi/keypress", 30);

   for(char *c = a->p->txtbuf; *c; c++)
   {
      switch(*c)
      {
      case '\b':
         if(st->tbptr - 1 >= 0)
            st->tbptr--;
         break;
      case '\r':
         *c = '\n';
      default:
         if(st->tbptr + 1 < Lith_CPS_Count(st->txtbuf) && (isprint(*c) || isspace(*c)))
         {
            Lith_CPS_SetC(st->txtbuf, st->tbptr, *c);
            st->tbptr++;
         }
         break;
      }
   }

   Lith_CPS_SetC(st->txtbuf, st->tbptr, '\0');

   PrintSprite(":UI:TextBoxBack", a->x-3 + g->ox,1, a->y-3 + g->oy,1);

   SetClipW(a->x + g->ox, a->y + g->oy, 260, 200, 260);
   if(st->tbptr)
      PrintTextFmt("%.*S%S", st->tbptr, Lith_CPS_Print(st->txtbuf), hot ? Ticker("|", "") : "");
   else
      PrintTextFmt("\C%c%S", hot ? 'c' : 'm', L("LITH_GUI_TEXTBOX"));
   PrintText("cbifont", CR_WHITE, a->x + g->ox,1, a->y + g->oy,1);
   ClearClip();

   memset(a->p->txtbuf, 0, sizeof a->p->txtbuf);
   a->p->tbptr = 0;

   return st;
}

// EOF

