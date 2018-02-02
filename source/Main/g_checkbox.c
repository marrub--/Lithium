// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"

//
// Lith_GUI_Checkbox_Impl
//
bool Lith_GUI_Checkbox_Impl(gui_state_t *g, id_t id, gui_checkb_args_t const *a)
{
   gui_checkb_preset_t const *pre = a->preset ? a->preset : &guipre.cbxdef;

   if(!a->disabled)
      Lith_GUI_Auto(g, id, a->x-(pre->w/2), a->y-(pre->h/2), pre->w, pre->h);

   __with(__str gfx    = Lith_GUI_Prefix1(g, pre, gfx);
          __str chkgfx = Lith_GUI_Prefix1(g, pre, chkgfx);)
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

