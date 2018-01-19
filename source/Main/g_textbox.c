// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

#include <ctype.h>

//
// Lith_GUI_TextBox_Impl
//
gui_txtbox_state_t *Lith_GUI_TextBox_Impl(gui_state_t *g, id_t id, gui_txtbox_args_t const *a)
{
   gui_txtbox_state_t *st = &g->st[a->st].tb;

   Lith_GUI_Auto(g, id, a->x, a->y, 8 * countof(st->txtbuf), 10);

   bool hot = g->hot == id;
   if(hot)
      HERMES("SetInput", a->pnum, true);

   if(*a->inbuf)
      ACS_LocalAmbientSound("player/cbi/keypress", 30);

   for(char *c = a->inbuf; *c; c++)
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
         if(st->tbptr + 1 < countof(st->txtbuf) && (isprint(*c) || isspace(*c)))
            st->txtbuf[st->tbptr++] = *c;
         break;
      }
   }

   st->txtbuf[st->tbptr] = '\0';

   DrawSpritePlain("lgfx/UI/TextBoxBack.png", g->hid--, (a->x - 3) + g->ox + .1, (a->y - 3) + g->oy + .1, TICSECOND);

   if(st->tbptr)
      HudMessageF("CBIFONT", "%.*s%S", st->tbptr, st->txtbuf, hot ? Ticker("|", "") : "");
   else
      HudMessageF("CBIFONT", "\C%cType here...", hot ? 'c' : 'm');

   HudMessagePlain(g->hid--, a->x + g->ox + .1, a->y + g->oy + .1, TICSECOND);

   return st;
}

// EOF

