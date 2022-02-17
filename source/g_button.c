// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ GUI buttons.                                                             │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"

bool G_Button_Imp(struct gui_state *g, gid_t id, struct gui_arg_btn *a) {
   struct gui_pre_btn const *pre = a->preset |? &gui_p.btndef;

   if(!a->disabled)
      G_Auto(g, id, a->x, a->y, pre->w, pre->h, a->slide);

   __with(char graphic[30];) {
      if(g->hot == id && pre->hot) G_Prefix(g, graphic, pre, hot);
      else                         G_Prefix(g, graphic, pre, gfx);

      if(graphic[0]) PrintSprite(fast_strdup(graphic), a->x + g->ox,1, a->y + g->oy,1);
   }

   if(a->label && pre->font) {
      i32 x, y;
      cstr color;

      if(pre->ax == 4 || !pre->ax) x = (pre->w / 2) + a->x + g->ox;
      else                         x = a->x + g->ox;

      if(pre->ay == 4 || !pre->ay) y = (pre->h / 2) + a->y + g->oy;
      else                         y = a->y + g->oy;

           if(a->disabled)     color = pre->cdis;
      else if(g->active == id) color = pre->cact;
      else if(g->hot    == id) color = pre->chot;
      else if(a->color)        color = a->color;
      else                     color = pre->cdef;
      if(!color) color = "-";

      PrintTextFmt("\C%s%s", color, a->label);
      PrintText(pre->font, g->defcr, x,pre->ax, y,pre->ay);
   }

   if(!a->disabled) {
      bool click;

      if(a->fill && a->fill->tic) {
         if(g->hot == id && g->active == id) {
            click = G_Filler(g->cx + 8, g->cy, a->fill, click);
         } else {
            if(g->active != id) {
               a->fill->cur = 0;
            }
            click = false;
         }
      } else if(g->slide == id) {
         click = g->clicklft && !g->old.clicklft;

              if(g->slidecount <  2) click = click || g->slidetime % 20 == 0;
         else if(g->slidecount < 10) click = click || g->slidetime % 5  == 0;
         else                        click = true;
      } else {
         click = !g->clicklft;
      }

      click = g->hot == id && g->active == id && click;

      if(click) {
         if(g->slide == id) g->slidecount++;
         if(pre->snd) AmbientSound(pre->snd, 1.0);
         return true;
      }
   }

   return false;
}

/* EOF */
