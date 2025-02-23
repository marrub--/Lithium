// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ GUI buttons.                                                             │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"

bool G_Button_Imp(struct gui_state *g, gid_t id, struct gui_arg_btn *a) {
   struct gui_pre_btn const *pre = a->preset |? &gui_p.btndef;

   if(!a->disabled) {
      G_Auto(g, id, a->x, a->y, pre->w, pre->h, a->slide);
   }

   __with(char graphic[30];) {
      if(g->hot == id && pre->hot) G_Prefix(g, graphic, pre, hot);
      else                         G_Prefix(g, graphic, pre, gfx);

      if(graphic[0]) PrintSprite(fast_strdup(graphic), a->x + g->ox,1, a->y + g->oy,1);
   }

   if(a->label && pre->font) {
      i32 x, y;
      i32 color;

      if(pre->ax == 4 || !pre->ax) x = (pre->w / 2) + a->x + g->ox;
      else                         x = a->x + g->ox;

      if(pre->ay == 4 || !pre->ay) y = (pre->h / 2) + a->y + g->oy;
      else                         y = a->y + g->oy;

           if(a->disabled)     color = pre->cdis;
      else if(g->active == id) color = pre->cact;
      else if(g->hot    == id) color = pre->chot;
      else if(a->color)        color = a->color;
      else                     color = pre->cdef;
      /* unfortunately, due to a series of bad API decisions, this
         must accept both 0 and -1, even though 0 = CR_BRICK. don't
         pass anything but -1 or not CR_BRICK i guess */
      if(color <= 0) color = g->defcr;

      PrintText_str(fast_strdup(a->label), pre->font, color, x,pre->ax, y,pre->ay);
   }

   if(!a->disabled) {
      bool click;

      if(a->fill && a->fill->tic) {
         if(g->hot == id && g->active == id) {
            if(g->old.hot != id || g->old.active != id) {
               a->fill->cur = 0;
            }
            click = G_Filler(g->cx - 19, g->cy + 4, a->fill, click);
         } else {
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
