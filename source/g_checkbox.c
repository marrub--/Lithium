// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ GUI checkboxes.                                                          │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"

bool G_ChkBox_Imp(struct gui_state *g, gid_t id, struct gui_arg_cbx *a) {
   struct gui_pre_cbx const *pre = a->preset |? &gui_p.cbxdef;

   if(!a->disabled) {
      G_Auto(g, id, a->x, a->y, pre->w, pre->h);
   }

   __with(char bgfx[30], cgfx[30];) {
      if(g->hot == id && pre->hot) G_Prefix(g, bgfx, pre, hot);
      else                         G_Prefix(g, bgfx, pre, gfx);

      /**/ if(g->active == id && pre->chkact) G_Prefix(g, cgfx, pre, chkact);
      else if(g->hot    == id && pre->chkhot) G_Prefix(g, cgfx, pre, chkhot);
      else                                    G_Prefix(g, cgfx, pre, chkgfx);

      if(!a->on) cgfx[0] = '\0';

      i32 cbx = a->x + pre->cx;
      i32 cby = a->y + pre->cy;

      if(bgfx[0]) PrintSprite(fast_strdup(bgfx), a->x + g->ox,1, a->y + g->oy,1);
      if(cgfx[0]) PrintSprite(fast_strdup(cgfx),  cbx + g->ox,1,  cby + g->oy,1);
   }

   if(g->hot == id && g->active == id && !g->clicklft) {
      if(a->on) {if(pre->snddn) AmbientSound(pre->snddn, 1.0);}
      else      {if(pre->sndup) AmbientSound(pre->sndup, 1.0);}
      return true;
   } else {
      return false;
   }
}

/* EOF */
