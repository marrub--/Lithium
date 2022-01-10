/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * GUI windows.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"

void G_WinBeg_Imp(struct gui_state *g, gid_t id, struct gui_arg_win *a) {
   struct gui_pre_win const *pre = a->preset |? &gui_p.windef;
   struct gui_win *win = a->st;

   if(!win->init) {
      i32 bx = g->w - pre->w;
      i32 by = g->h - pre->h;
      win->x = bx / 2;
      win->y = by / 2;
      win->init = true;
   }

   if(win->nextsnd) win->nextsnd--;

   i32 ox = g->ox + win->x;
   i32 oy = g->oy + win->y;

   G_Auto(g, id,
          ox, oy,
          pre->w, pre->by);
   G_Auto(g, id,
          ox, oy,
          pre->bx, pre->h);
   G_Auto(g, id,
          ox,
          oy + pre->h - pre->by,
          pre->w, pre->by);
   G_Auto(g, id,
          ox + pre->w - pre->bx,
          oy,
          pre->bx, pre->h);

   if(g->active == id) {
      win->x += g->cx - g->old.cx;
      win->y += g->cy - g->old.cy;
      win->x = minmax(win->x, 0, g->w - pre->w);
      win->y = minmax(win->y, 0, g->h - pre->h);
      ox = g->ox + win->x;
      oy = g->oy + win->y;

      if(g->cy != g->old.cy && win->nextsnd == 0) {
         AmbientSound(ss_player_cbi_scroll, 1.0);
         win->grabbed = true;
         win->nextsnd = 7;
      }
   } else if(win->grabbed) {
      AmbientSound(ss_player_cbi_scrollend, 1.0);
      win->grabbed = false;
   }

   __with(char bg[64];) {
      G_Prefix(g, bg, pre, bg);
      PrintSpriteA(fast_strdup(bg), ox,1, oy,1, pre->a);
   }

   ox += pre->bx;
   oy += pre->by;

   i32 const w = pre->w - pre->bx * 2;
   i32 const h = pre->h - pre->by * 2;

   g->ox += (win->ox = ox - g->ox);
   g->oy += (win->oy = oy - g->oy);

   G_Clip(g, g->ox, g->oy, w, h, w);
}

void G_WinEnd(struct gui_state *g, struct gui_win *win) {
   g->ox -= win->ox;
   g->oy -= win->oy;

   G_ClipRelease(g);
}

/* EOF */
