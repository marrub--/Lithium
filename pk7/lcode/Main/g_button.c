#include "lith_common.h"
#include "lith_player.h"

//
// Lith_GUI_Button_Impl
//
bool Lith_GUI_Button_Impl(gui_state_t *g, id_t id, gui_button_args_t const *a)
{
   gui_button_preset_t const *pre = a->preset ? a->preset : &btndefault;

   if(!a->disabled)
      Lith_GUI_Auto(g, id, a->x, a->y, pre->w, pre->h);

   __with(__str graphic;)
   {
      int const x = a->x + g->ox;
      int const y = a->y + g->oy;
      if(g->hot == id && pre->hot) graphic = Lith_GUI_Prefix1(g, pre, hot);
      else                         graphic = Lith_GUI_Prefix1(g, pre, gfx);

      if(graphic)
         DrawSpritePlain(graphic, g->hid--, x + 0.1, y + 0.1, TICSECOND);
   }

   if(a->label && pre->font)
   {
      int const x = (pre->w / 2) + a->x + g->ox;
      int const y = (pre->h / 2) + a->y + g->oy;
      __str color;

           if(a->disabled)     color = pre->cdis;
      else if(g->active == id) color = pre->cact;
      else if(g->hot    == id) color = pre->chot;
      else if(a->color)        color = a->color;
      else                     color = pre->cdef;
      color = color ? color : "j";

      HudMessageF(pre->font, "\C%S%S", color, a->label);
      HudMessagePlain(g->hid--, x + 0.4, y, TICSECOND);
   }

   if(g->hot == id && g->active == id && !g->clicklft) {
      if(pre->snd)
         ACS_LocalAmbientSound(pre->snd, 127);
      return true;
   } else {
      return false;
   }
}

// EOF

