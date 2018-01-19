// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"

//
// Lith_GUI_Button_Impl
//
bool Lith_GUI_Button_Impl(gui_state_t *g, id_t id, gui_button_args_t const *a)
{
   gui_button_preset_t const *pre = a->preset ? a->preset : &guipre.btndef;

   if(!a->disabled)
      Lith_GUI_Auto(g, id, a->x, a->y, pre->w, pre->h, a->slide);

   __with(__str graphic;)
   {
      if(g->hot == id && pre->hot) graphic = Lith_GUI_Prefix1(g, pre, hot);
      else                         graphic = Lith_GUI_Prefix1(g, pre, gfx);

      if(graphic) {
         int const x = a->x + g->ox;
         int const y = a->y + g->oy;
         DrawSpritePlain(graphic, g->hid--, x + 0.1, y + 0.1, TICSECOND);
      }
   }

   if(a->label && pre->font)
   {
      int x, y;
      __str color;

      if(pre->ax == 0.4 || !pre->ax) x = (pre->w / 2) + a->x + g->ox;
      else                           x = a->x + g->ox;

      if(pre->ay == 0.4 || !pre->ay) y = (pre->h / 2) + a->y + g->oy;
      else                           y = a->y + g->oy;

           if(a->disabled)     color = pre->cdis;
      else if(g->active == id) color = pre->cact;
      else if(g->hot    == id) color = pre->chot;
      else if(a->color)        color = a->color;
      else                     color = pre->cdef;
      color = color ? color : "j";

      HudMessageF(pre->font, "\C%S%S", color, a->label);
      HudMessagePlain(g->hid--, x + pre->ax, y + pre->ay, TICSECOND);
   }

   if(!a->disabled)
   {
      bool click = !g->clicklft;

      if(g->slide == id)
      {
         click = g->clicklft && !g->old.clicklft;

              if(g->slidecount < 10) click = click || g->slidetime % 5 == 0;
         else if(g->slidecount < 20) click = click || g->slidetime % 2 == 0;
         else                        click = true;
      }

      if(g->hot == id && g->active == id && click) {
         if(g->slide == id) g->slidecount++;
         if(pre->snd) ACS_LocalAmbientSound(pre->snd, 127);
         return true;
      }
   }

   return false;
}

// EOF
