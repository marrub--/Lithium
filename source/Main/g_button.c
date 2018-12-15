// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"

StrEntON

bool Lith_GUI_Button_Impl(gui_state_t *g, id_t id, gui_button_args_t const *a)
{
   gui_button_preset_t const *pre = a->preset ? a->preset : &guipre.btndef;

   if(!a->disabled)
      Lith_GUI_Auto(g, id, a->x, a->y, pre->w, pre->h, a->slide);

   __with(char graphic[30];)
   {
      if(g->hot == id && pre->hot) Lith_GUI_Prefix2(g, graphic, pre, hot);
      else                         Lith_GUI_Prefix2(g, graphic, pre, gfx);

      if(graphic[0]) PrintSprite(l_strdup(graphic), a->x + g->ox,1, a->y + g->oy,1);
   }

   if(a->label && pre->font)
   {
      int x, y;
      char const *color;

      if(pre->ax == 4 || !pre->ax) x = (pre->w / 2) + a->x + g->ox;
      else                         x = a->x + g->ox;

      if(pre->ay == 4 || !pre->ay) y = (pre->h / 2) + a->y + g->oy;
      else                         y = a->y + g->oy;

           if(a->disabled)     color = pre->cdis;
      else if(g->active == id) color = pre->cact;
      else if(g->hot    == id) color = pre->chot;
      else if(a->color)        color = a->color;
      else                     color = pre->cdef;
      if(!color) color = c"j";

      PrintTextFmt("\C%s%s", color, a->label);
      PrintText(l_strdup(pre->font), CR_WHITE, x,pre->ax, y,pre->ay);
   }

   if(!a->disabled)
   {
      bool click = !g->clicklft;

      if(g->slide == id)
      {
         click = g->clicklft && !g->old.clicklft;

              if(g->slidecount < 2)  click = click || g->slidetime % 20 == 0;
         else if(g->slidecount < 10) click = click || g->slidetime % 5  == 0;
         else                        click = true;
      }

      if(g->hot == id && g->active == id && click) {
         if(g->slide == id) g->slidecount++;
         if(pre->snd) ACS_LocalAmbientSound(l_strdup(pre->snd), 127);
         return true;
      }
   }

   return false;
}

// EOF
