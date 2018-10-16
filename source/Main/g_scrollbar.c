// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"

// This function is excessively commented in case I need to change it.
// Reading this function will cause a 5x1d20 hit to your SAN stat. Beware!
void Lith_GUI_ScrollBegin_Impl(gui_state_t *g, id_t id, gui_scroll_args_t const *a)
{
   gui_scroll_preset_t const *pre = a->preset ? a->preset : &guipre.scrdef;
   gui_scroll_state_t *scr = a->st;

   // sizes
   int const blockh  = pre->scrlh;        // height of graphical block
   int const blocks  = a->h / blockh - 1; // height in graphical blocks -caps
   int const caph    = blockh / 2;        // size of cap
   int const caps    = blocks * 2;        // height in caps, -caps
   int const h       = blocks * blockh;   // height in pixels -caps
   int const realh   = h + caph * 2;      // height in pixels +caps

   // positions
   int x = a->x + pre->scrlw; // base x to draw from
   int y = a->y;              // base y to draw from

   // check collision - height is minus caps, and y is offset by the top cap
   Lith_GUI_Auto(g, id, x - pre->scrlw, y + caph, pre->scrlw, h);

   // add offset
   x += g->ox;
   y += g->oy;

   // get height of scroller
   int notches; // height of scroller in caps

   if(a->contenth > realh) notches = a->h / (fixed64)a->contenth * caps;
   else                    notches = caps;

   int     const scrlh = notches * caph;           // height in pixels of scroller
   fixed64 const maxy  = (h - scrlh) / (fixed64)h; // normalized maximum y value

   // decrement the sound timer
   if(scr->nextsnd) scr->nextsnd--;

   // move scroller
   __with(fixed64 supposedy = scr->y * h;)
   {
      if(g->active == id)
      {
         fixed64 const cy = g->cy - y - caph;

         // if it isn't grabbed and the cursor is over the scroller,
         // set the grab position to where the cursor is relative to it
         if(!scr->grabbed && cy > supposedy && cy < supposedy + scrlh)
         {
            scr->grabbed = true;
            scr->grabpos = cy - supposedy;
         }

         // if the scroller is grabbed we set the position relative to where
         // we grabbed it from, otherwise we just use the middle of it
         if(scr->grabbed) supposedy = cy - scr->grabpos;
         else             supposedy = cy - scrlh / 2;

         if(g->cy != g->old.cy && scr->nextsnd == 0)
         {
            ACS_LocalAmbientSound("player/cbi/scroll", 127);
            scr->nextsnd = 7;
         }
      }
      else if(scr->grabbed)
      {
         ACS_LocalAmbientSound("player/cbi/scrollend", 127);
         scr->grabbed = false;
         scr->nextsnd = 0;
      }

      // finally, normalize and clamp
      scr->y = supposedy / (fixed64)h;
      scr->y = minmax(scr->y, 0, maxy);
   }

   // get offset of scroller
   __with(int vofs = 0;) // offset in pixels of the content
   {
      if(a->contenth > realh)
         vofs = roundlk((a->contenth - realh) * (scr->y / maxy), 10);

      // set the scrollbar's offset
      scr->ox = a->x + pre->scrlw; // offset by scrollbar width
      scr->oy = a->y - vofs;       // offset by scroller pos

      // set the top and bottom for occlusion
      scr->occludeS = vofs;
      scr->occludeE = vofs + realh;
   }

   // draw top cap
   ifauto(__str, cap, Lith_GUI_Prefix1(g, pre, capS))
      PrintSprite(cap, x,2, y,1);
   y += caph;

   // draw middle of bar
   __with(__str scrl = Lith_GUI_Prefix1(g, pre, scrl);)
      for(int i = 0; i < blocks; i++)
   {
      if(scrl) PrintSprite(scrl, x,2, y,1);
      y += blockh;
   }

   // draw bottom cap
   ifauto(__str, cap, Lith_GUI_Prefix1(g, pre, capE))
      PrintSprite(cap, x,2, y,1);

   // get base Y
   int const ory = a->y + g->oy;

   // draw scroller
   __with(__str graphic;)
   {
      if(g->hot == id || g->active == id)
         graphic = Lith_GUI_Prefix1(g, pre, notchhot);
      else
         graphic = Lith_GUI_Prefix1(g, pre, notchgfx);

      if(graphic) for(int i = 0; i < notches; i++)
      {
         int const npos = roundlk(caph + h * scr->y + caph * i, 10);
         PrintSprite(graphic, x,2, ory + npos,1);
      }
   }

   // setup offsets
   g->ox += scr->ox;
   g->oy += scr->oy;

   // setup clip
   Lith_GUI_Clip(g, x, ory, a->w, realh, a->ww);
}

void Lith_GUI_ScrollEnd(gui_state_t *g, gui_scroll_state_t *scr)
{
   // reset offsets
   g->ox -= scr->ox;
   g->oy -= scr->oy;

   // reset clip
   Lith_GUI_ClipRelease(g);
}

bool Lith_GUI_ScrollOcclude(gui_state_t *g, gui_scroll_state_t const *scr, int y, int h)
{
   return y > scr->occludeE || (h && (y + h) - scr->occludeS < 0);
}

// EOF

