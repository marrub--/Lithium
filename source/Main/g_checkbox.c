// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// g_checkbox.c: GUI checkboxes.

#include "common.h"
#include "p_player.h"

bool Lith_GUI_Checkbox_Impl(struct gui_state *g, u32 id, struct gui_arg_cbx const *a)
{
   struct gui_pre_cbx const *pre = a->preset ? a->preset : &gui_p.cbxdef;

   if(!a->disabled)
      Lith_GUI_Auto(g, id, a->x-(pre->w/2), a->y-(pre->h/2), pre->w, pre->h);

   __with(char gfx[64], chkgfx[64];)
   {
      if(a->disabled) {
         if(pre->dis)    Lith_GUI_Prefix(g, gfx,    pre, dis);
         if(pre->chkdis) Lith_GUI_Prefix(g, chkgfx, pre, chkdis);
      } else if(g->active == id) {
         if(pre->hot)    Lith_GUI_Prefix(g, gfx,    pre, hot);
         if(pre->chkact) Lith_GUI_Prefix(g, chkgfx, pre, chkact);
      } else if(g->hot == id) {
         if(pre->hot)    Lith_GUI_Prefix(g, gfx,    pre, hot);
         if(pre->chkhot) Lith_GUI_Prefix(g, chkgfx, pre, chkhot);
      }

      if(!gfx[0])    Lith_GUI_Prefix(g, gfx,    pre, gfx);
      if(!chkgfx[0]) Lith_GUI_Prefix(g, chkgfx, pre, chkgfx);

      if(gfx[0])             PrintSprite(l_strdup(gfx),    a->x + g->ox,0, a->y + g->oy,0);
      if(chkgfx[0] && a->on) PrintSprite(l_strdup(chkgfx), a->x + g->ox,0, a->y + g->oy,0);
   }

   if(g->hot == id && g->active == id && !g->clicklft)
   {
      if(a->on) {if(pre->snddn) ACS_LocalAmbientSound(pre->snddn, 127);}
      else      {if(pre->sndup) ACS_LocalAmbientSound(pre->sndup, 127);}
      return true;
   }
   else
      return false;
}

// EOF
