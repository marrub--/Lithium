#include "lith_common.h"
#include "lith_player.h"


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_GUI_Button_Impl
//
bool Lith_GUI_Button_Impl(gui_state_t *g, id_t id, gui_button_args_t *a)
{
   Lith_GUI_GenPreset(gui_button_preset_t, btndefault);
   
   if(!a->disabled)
      Lith_GUI_Auto(g, id, a->x, a->y, pre->w, pre->h);
   
   {
   __str graphic;
   
   if(g->hot == id && pre->hot) graphic = pre->hot;
   else                         graphic = pre->gfx;
   
   if(graphic)
      DrawSpritePlain(graphic, g->hid--, a->x + g->ox + 0.1, a->y + g->oy + 0.1, TICSECOND);
   }
   
   if(a->label)
   {
      __str color;
      
           if(a->disabled)     color = pre->cdis;
      else if(g->active == id) color = pre->cact;
      else if(g->hot    == id) color = pre->chot;
      else if(a->color)        color = a->color;
      else                     color = pre->cdef;
      
      HudMessageF(pre->font, "\C%S%S", color, a->label);
      HudMessagePlain(g->hid--, (pre->w / 2) + a->x + g->ox + 0.4, (pre->h / 2) + a->y + g->oy, TICSECOND);
   }
   
   if(g->hot == id && g->active == id && !g->clicklft)
   {
      if(pre->snd) ACS_LocalAmbientSound(pre->snd, 127);
      return true;
   }
   else return false;
}

// EOF

