#include "lith_common.h"
#include "lith_player.h"


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_GUI_ScrollBegin_Impl
//
void Lith_GUI_ScrollBegin_Impl(gui_state_t *g, id_t id, gui_scroll_args_t *a)
{
   Lith_GUI_GenPreset(gui_scroll_preset_t, scrdefault);
   
   gui_scroll_state_t *scr = &g->st[a->st].scrl; // scrollbar state
   
   // sizes
   int caph   = pre->scrlh / 2;          // size of cap
   int blocks = (a->h / pre->scrlh) - 1; // height in graphical blocks minus caps
   int h      = (blocks * pre->scrlh);   // height in pixels minus caps
   int realh  = h + pre->scrlh;          // height in pixels plus caps
   int vofs   = 0; // offset in pixels of the content
   
   if(a->contenth > realh)
      vofs = (a->contenth - realh) * scr->y;
   
   // set the scrollbar's offset
   scr->ox = a->x + pre->scrlw; // offset by scrollbar width
   scr->oy = a->y - vofs;       // offset by scrollbar notch pos
   
   // convenience variables
   int x = a->x + pre->scrlw; // base x to draw from
   int y = a->y;              // base y to draw from
   
   // check collision - height is minus caps, and y is offset by the top cap
   Lith_GUI_Auto(g, id, x - pre->scrlw, y + caph, pre->scrlw, h);
   
   // add offset
   x += g->ox;
   y += g->oy;
   
   int ory = y; // copy of y since it'll be changed later
   
   // move scroll notch
   if(g->active == id)
   {
      // needs to be two expressions because minmax copies the expression
      scr->y = ((g->cy - y) - caph) / (double)h;
      scr->y = minmax(scr->y, 0, 1);
   }
   
   // draw top cap
   DrawSpritePlain(pre->capS, g->hid--, x + 0.2, y + 0.1, TICSECOND);
   y += caph;
   
   // draw middle of bar
   for(int i = 0; i < blocks; i++)
   {
      DrawSpritePlain(pre->scrl, g->hid--, x + 0.2, y + 0.1, TICSECOND);
      y += pre->scrlh;
   }
   
   // draw bottom cap
   DrawSpritePlain(pre->capE, g->hid--, x + 0.2, y + 0.1, TICSECOND);
   
   // set the top and bottom for occlusion
   scr->occludeS = vofs;
   scr->occludeE = realh + vofs;
   
   // draw notch
   {
   __str graphic = g->hot == id || g->active == id ? pre->notchhot : pre->notchgfx;
   DrawSpritePlain(graphic, g->hid--, x + 0.2, ory + (int)(h * scr->y) + caph, TICSECOND);
   }
   
   // setup offsets
   g->ox += scr->ox;
   g->oy += scr->oy;
   
   // setup clip
   Lith_GUI_Clip(g, x, ory, a->w, realh);
}

//
// Lith_GUI_ScrollEnd
//
void Lith_GUI_ScrollEnd(gui_state_t *g, size_t st)
{
   gui_scroll_state_t *scr = &g->st[st].scrl;
   
   // reset offsets 
   g->ox -= scr->ox;
   g->oy -= scr->oy;
   
   // reset clip
   Lith_GUI_ClipRelease(g);
}

//
// Lith_GUI_ScrollOcclude
//
bool Lith_GUI_ScrollOcclude(gui_state_t *g, size_t st, int y, int h)
{
   gui_scroll_state_t const *scr = &g->st[st].scrl;
   
   return
      y > scr->occludeE ||                // too low to be seen
      (h && (y + h) - scr->occludeS < 0); // too high to be seen (if height is available)
}

// EOF

