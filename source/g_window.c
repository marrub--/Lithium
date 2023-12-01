// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ GUI windows.                                                             │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
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
   __with(char bg[64];) {
      G_Prefix(g, bg, pre, bg);
      PrintSprite(fast_strdup(bg), g->ox+win->x,1, g->oy+win->y,1, _u_alpha, pre->a);
   }
   i32 const w = pre->w - pre->bx * 2;
   i32 const h = pre->h - pre->by * 2;
   g->ox += (win->ox = win->x + pre->bx);
   g->oy += (win->oy = win->y + pre->by);
   G_Clip(g, 0, 0, w, h, w);
}

void G_WinEnd_Imp(struct gui_state *g, gid_t id, struct gui_arg_win *a) {
   struct gui_pre_win const *pre = a->preset |? &gui_p.windef;
   struct gui_win *win = a->st;
   g->ox -= win->ox;
   g->oy -= win->oy;
   G_ClipRelease(g);
   i32 const ox = g->ox + win->x;
   i32 const oy = g->oy + win->y;
   G_Auto(g, id, ox, oy, pre->w, pre->by);
   G_Auto(g, id, ox, oy, pre->bx, pre->h);
   G_Auto(g, id, ox, oy + pre->h - pre->by, pre->w, pre->by);
   G_Auto(g, id, ox + pre->w - pre->bx, oy, pre->bx, pre->h);
   if(g->active == id) {
      k64 oldx = win->x, oldy = win->y;
      win->x += g->cx - g->old.cx;
      win->y += g->cy - g->old.cy;
      win->x = clamplk(win->x, 0, g->w - pre->w);
      win->y = clamplk(win->y, 0, g->h - pre->h);
      if(win->x == oldx) {
         g->cx = g->old.cx;
      }
      if(win->y == oldy) {
         g->cy = g->old.cy;
      }
      if(g->cy != g->old.cy && win->nextsnd == 0) {
         AmbientSound(ss_player_cbi_scroll, 1.0);
         win->grabbed = true;
         win->nextsnd = 7;
      }
   } else if(win->grabbed) {
      AmbientSound(ss_player_cbi_scrollend, 1.0);
      win->grabbed = false;
   }
}

/* EOF */
