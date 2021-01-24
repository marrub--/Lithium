/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * GUI sliders.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"

struct slide_ret G_Slider_Imp(struct gui_state *g, u32 id, struct gui_arg_sld const *a) {
   struct gui_pre_sld const *pre = a->preset ? a->preset : &gui_p.slddef;

   k32 w = pre->w - (pre->pad * 2);

   i32 x = a->x + pre->pad;
   i32 y = a->y;

   G_Auto(g, id, x, y, w, pre->h);

   x += g->ox;
   y += g->oy;

   k32  ret;
   k32  notchpos;
   bool big = a->maxima - a->minima > 2.0k;

   /* get scroll notch and handle inputs */
   if(g->active == id) {
      notchpos = g->cx - x;
      notchpos = minmax(notchpos, 0, w) / w;
      ret      = lerpk(a->minima, a->maxima, notchpos);
      if(big) ret = fastround1k(ret);
      else    ret = fastround1k(ret * 10.0k) / 10.0k;

      /* play sound */
      if(pre->snd && g->cx != g->old.cx && g->cx >= x && g->cx < x + w)
         ACS_LocalAmbientSound(pre->snd, 60);
   } else {
      notchpos = (a->val - a->minima) / a->maxima;
      ret      = a->val;
   }

   /* draw graphic */
   __with(char gfx[64];) {
      G_Prefix(g, gfx, pre, gfx);
      if(gfx[0]) PrintSprite(l_strdup(gfx), x - pre->pad,1, y + pre->h / 2,0);
   }

   /* draw notch */
   __with(char gfx[64];) {
      if(g->hot == id || g->active == id) G_Prefix(g, gfx, pre, notchhot);
      else                                G_Prefix(g, gfx, pre, notch);

      if(gfx[0]) PrintSprite(l_strdup(gfx), x + notchpos * w - 1,1, y,1);
   }

   /* draw value */
   if(pre->font) {
      cstr suf = a->suf ? a->suf : "";
      if(a->integ) PrintTextFmt("%i%s",                (i32)ret, suf);
      else         PrintTextFmt("%.*k%s", big ? 1 : 2, (k32)ret, suf);

      PrintText(pre->font, g->defcr, x + pre->w/2,4, y + pre->h/2,0);
   }

   struct slide_ret sret = {
      .different = g->active == id && !g->clicklft && ret != a->val,
      .value     = ret,
   };
   return sret;
}

/* EOF */
