// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ GUI sliders.                                                             │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"

struct slide_ret G_Slider_Imp(struct gui_state *g, gid_t id, struct gui_arg_sld *a) {
   struct gui_pre_sld const *pre = a->preset |? &gui_p.slddef;
   i32 w = pre->w - (pre->pad * 2);
   i32 x = a->x + pre->pad;
   i32 y = a->y;
   G_Auto(g, id, x, y, w, pre->h);
   x += g->ox;
   y += g->oy;
   bool intstep = fastfraclk(a->step) == 0;
   i32 val = intstep ? a->val    : (i32)(a->val   /a->step);
   i32 min = intstep ? a->minima : (i32)(a->minima/a->step);
   i32 max = intstep ? a->maxima : (i32)(a->maxima/a->step);
   i32 intval;
   k64 notchpos;
   /* get scroll notch and handle inputs */
   if(g->active == id) {
      notchpos = clamplk(g->cx - x, 0, w) / w;
      intval   = lerplk(min, max, notchpos);
      if(intstep) intval = intval - intval % (i32)a->step;
      /* play sound */
      if(pre->snd && g->cx != g->old.cx && g->cx >= x && g->cx < x + w) {
         AmbientSound(pre->snd, 0.47);
      }
   } else {
      notchpos = (val - min) / (k64)(max - min);
      intval   = val;
   }
   /* draw graphic */
   __with(char gfx[64];) {
      G_Prefix(g, gfx, pre, gfx);
      if(gfx[0]) PrintSprite(fast_strdup(gfx), x - pre->pad,1, y + pre->h / 2,0);
   }
   /* draw notch */
   __with(char gfx[64];) {
      if(g->hot == id || g->active == id) G_Prefix(g, gfx, pre, notchhot);
      else                                G_Prefix(g, gfx, pre, notch);
      if(gfx[0]) PrintSprite(fast_strdup(gfx), x + (i32)(notchpos * w) - 1,1, y,1);
   }
   k64 value;
   if(intstep) value = intval;
   else        value = a->step * intval;
   /* draw value */
   if(pre->font) {
      cstr suf = a->suf |? "";
      if(intstep) {ACS_BeginPrint(); ACS_PrintInt(value); PrintStr(suf);}
      else        {BeginPrintFmt("%.2k", (k32)(value * 1.001lk)); PrintStr(suf);}
      PrintText(pre->font, g->defcr, x + pre->w/2,4, y + pre->h/2,0);
   }
   struct slide_ret sret = {
      .different = g->active == id && !g->clicklft && value != a->val,
      .value     = value,
   };
   return sret;
}

/* EOF */
