// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"

fixed64 Lith_GUI_Slider_Impl(gui_state_t *g, id_t id, gui_slider_args_t const *a)
{
   gui_slider_preset_t const *pre = a->preset ? a->preset : &guipre.slddef;

   fixed64 w = pre->w - (pre->pad * 2);

   int x = a->x + pre->pad;
   int y = a->y;

   Lith_GUI_Auto(g, id, x, y, w, pre->h);

   x += g->ox;
   y += g->oy;

   // get a normalized value
   fixed64 aval;

   aval = (a->val - a->minima) / (a->maxima - a->minima);
   aval = minmax(aval, 0, 1);

   fixed64 val;

   // move scroll notch
   if(g->active == id)
   {
      val  = g->cx - x;
      val  = minmax(val, 0, w);
      val /= w;

      // play sound
      if(pre->snd && g->cx != g->old.cx && g->cx >= x && g->cx < x + w)
         ACS_LocalAmbientSound(pre->snd, 60);
   }
   else
      val = aval;

   // get result-normalized value
   fixed64 norm = val * (a->maxima - a->minima) + a->minima;

   if(a->integ) norm = roundlk(norm, 10);

   // draw graphic
   ifauto(__str, gfx, Lith_GUI_Prefix1(g, pre, gfx))
      PrintSprite(gfx, x - pre->pad,1, y + pre->h / 2,0);

   // draw notch
   __with(__str graphic;)
   {
      if(g->hot == id || g->active == id)
         graphic = Lith_GUI_Prefix1(g, pre, notchhot);
      else
         graphic = Lith_GUI_Prefix1(g, pre, notch);

      if(graphic)
         PrintSprite(graphic, x + val * w - 1,1, y,1);
   }

   // draw value
   if(pre->font)
   {
      __str suf = a->suf ? a->suf : "";
      fixed64 amt = roundlk(norm * 100.lk, 10) / 100.lk;
      if(a->integ) PrintTextFmt("%i%S",     (int)amt, suf);
      else         PrintTextFmt("%.1k%S", (fixed)amt, suf);

      PrintText(pre->font, CR_WHITE, x + pre->w/2,4, y + pre->h/2,0);
   }

   // if we've moved it, we return a difference
   if(g->active == id && !g->clicklft && !CloseEnough(aval, val))
      return norm - a->val;
   else
      return 0;
}

// EOF

