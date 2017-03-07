#include "lith_common.h"
#include "lith_player.h"


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_GUI_Checkbox_Impl
//
bool Lith_GUI_Checkbox_Impl(gui_state_t *g, id_t id, gui_checkb_args_t *a)
{
   Lith_GUI_GenPreset(gui_checkb_preset_t, cbxdefault);
   
   if(!a->disabled)
      Lith_GUI_Auto(g, id, a->x - (pre->w/2), a->y - (pre->h/2), pre->w, pre->h);
   
   {
   __str gfx = pre->gfx, chkgfx = pre->chkgfx;
   
   if(g->active == id)
   {
      if(pre->hot)    gfx    = pre->hot;
      if(pre->chkact) chkgfx = pre->chkact;
   }
   else if(g->hot == id)
   {
      if(pre->hot)    gfx    = pre->hot;
      if(pre->chkhot) chkgfx = pre->chkhot;
   }
   
   if(gfx)
      DrawSpritePlain(gfx, g->hid--, a->x + g->ox, a->y + g->oy, TICSECOND);
   
   if(chkgfx && a->on)
      DrawSpritePlain(chkgfx, g->hid--, a->x + g->ox, a->y + g->oy, TICSECOND);
   }
   
   if(a->label)
   {
      // I know that this is positioned wrong, but in the only place it's used
      // currently it looks good.
      HudMessageF(pre->font, "%S", a->label);
      HudMessagePlain(g->hid--, (pre->w / 2) + a->x + g->ox + 0.4, (pre->h / 2) + a->y + g->oy, TICSECOND);
   }
   
   if(g->hot == id && g->active == id && !g->clicklft)
   {
      if(a->on) {if(pre->snddn) ACS_LocalAmbientSound(pre->snddn, 127);}
      else      {if(pre->sndup) ACS_LocalAmbientSound(pre->sndup, 127);}
      return true;
   }
   else return false;
}

// EOF

