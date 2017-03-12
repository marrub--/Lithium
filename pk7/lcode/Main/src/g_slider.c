#include "lith_common.h"
#include "lith_player.h"

#include <math.h>


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_GUI_Slider_Impl
//
double Lith_GUI_Slider_Impl(gui_state_t *g, id_t id, gui_slider_args_t *a)
{
   Lith_GUI_GenPreset(gui_slider_preset_t, slddefault);
   
   double w = pre->w - (pre->pad * 2);
   
   int x = a->x + pre->pad;
   int y = a->y;
   
   Lith_GUI_Auto(g, id, x, y, w, pre->h);
   
   x += g->ox;
   y += g->oy;
   
   // get a normalized value
   double aval;
   
   aval = (a->val - a->minima) / (a->maxima - a->minima);
   aval = minmax(aval, 0, 1);
   
   double val;
   
   // move scroll notch
   if(g->active == id)
   {
      val  = g->cx - x;
      val  = minmax(val, 0, w);
      val /= w;
      
      // play sound
      if(g->cx != g->old.cx && g->cx >= x && g->cx < x + w)
         ACS_LocalAmbientSound(pre->snd, 60);
   }
   else
      val = aval;
   
   // get result-normalized value
   double norm = val * (a->maxima - a->minima) + a->minima;
   
   if(a->integ) norm = round(norm);
   
   // draw graphic
   DrawSpritePlain(pre->gfx, g->hid--, (x - pre->pad) + 0.1, y + (pre->h / 2), TICSECOND);
   
   // draw notch
   {
   __str graphic;
   
   if(g->hot == id || g->active == id) graphic = pre->notchhot;
   else                                graphic = pre->notch;
   
   DrawSpritePlain(graphic, g->hid--, x + (int)(val * w) - 1 + 0.1, y + 0.1, TICSECOND);
   }
   
   // draw value
   if(a->integ) HudMessageF("CBIFONT", "\Cj%i",     (int)(round(norm * 100.0) / 100.0));
   else         HudMessageF("CBIFONT", "\Cj%.1k", (fixed)(round(norm * 100.0) / 100.0));
   
   HudMessagePlain(g->hid--, x + (pre->w / 2) + 0.4, y + (pre->h / 2), TICSECOND);
   
   // if we've moved it, we return a difference
   if(g->active == id && !g->clicklft && !CloseEnough(aval, val))
      return norm - a->val;
   
   // otherwise we return 0
   else
      return 0;
}

// EOF

