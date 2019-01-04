// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#include "common.h"
#include "p_player.h"

bool Lith_GUI_Checkbox_Impl(struct gui_state *g, u32 id, struct gui_arg_cbx const *a)
{
   struct gui_pre_cbx const *pre = a->preset ? a->preset : &gui_p.cbxdef;

   if(!a->disabled)
      Lith_GUI_Auto(g, id, a->x-(pre->w/2), a->y-(pre->h/2), pre->w, pre->h);

   __with(str gfx    = Lith_GUI_Prefix1(g, pre, gfx);
          str chkgfx = Lith_GUI_Prefix1(g, pre, chkgfx);)
   {
      if(a->disabled) {
         if(pre->dis)    gfx    = Lith_GUI_Prefix1(g, pre, dis);
         if(pre->chkdis) chkgfx = Lith_GUI_Prefix1(g, pre, chkdis);
      } else if(g->active == id) {
         if(pre->hot)    gfx    = Lith_GUI_Prefix1(g, pre, hot);
         if(pre->chkact) chkgfx = Lith_GUI_Prefix1(g, pre, chkact);
      } else if(g->hot == id) {
         if(pre->hot)    gfx    = Lith_GUI_Prefix1(g, pre, hot);
         if(pre->chkhot) chkgfx = Lith_GUI_Prefix1(g, pre, chkhot);
      }

      if(gfx)
         PrintSprite(gfx, a->x + g->ox,0, a->y + g->oy,0);

      if(chkgfx && a->on)
         PrintSprite(chkgfx, a->x + g->ox,0, a->y + g->oy,0);
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
