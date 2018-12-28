// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "common.h"
#include "p_player.h"

// This function is excessively commented in case I need to change it.
// Reading this function will cause a 5x1d20 hit to your SAN stat. Beware!
void Lith_GUI_ScrollBegin_Impl(struct gui_state *g, u32 id, struct gui_arg_scr const *a)
{
   struct gui_pre_scr const *pre = a->preset ? a->preset : &gui_p.scrdef;
   struct gui_scr *scr = a->st;

   // sizes
   i32 const blockh = pre->scrlh;        // height of graphical block
   i32 const blocks = a->h / blockh - 1; // height in graphical blocks -caps
   i32 const caph   = blockh / 2;        // size of cap
   i32 const caps   = blocks * 2;        // height in caps, -caps
   i32 const h      = blocks * blockh;   // height in pixels -caps
   i32 const realh  = h + caph * 2;      // height in pixels +caps

   // positions
   i32 x = a->x + pre->scrlw; // base x to draw from
   i32 y = a->y;              // base y to draw from

   // check collision - height is minus caps, and y is offset by the top cap
   Lith_GUI_Auto(g, id, x - pre->scrlw, y + caph, pre->scrlw, h);

   // add offset
   x += g->ox;
   y += g->oy;

   // get height of scroller
   i32 notches; // height of scroller in caps

   if(a->contenth > realh) notches = a->h / (k64)a->contenth * caps;
   else                    notches = caps;

   i32     const scrlh = notches * caph;           // height in pixels of scroller
   k64 const maxy  = (h - scrlh) / (k64)h; // normalized maximum y value

   // decrement the sound timer
   if(scr->nextsnd) scr->nextsnd--;

   // move scroller
   __with(k64 supposedy = scr->y * h;)
   {
      if(g->active == id)
      {
         k64 const cy = g->cy - y - caph;

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
            ACS_LocalAmbientSound(ss_player_cbi_scroll, 127);
            scr->nextsnd = 7;
         }
      }
      else if(scr->grabbed)
      {
         ACS_LocalAmbientSound(ss_player_cbi_scrollend, 127);
         scr->grabbed = false;
         scr->nextsnd = 0;
      }

      // finally, normalize and clamp
      scr->y = supposedy / (k64)h;
      scr->y = minmax(scr->y, 0, maxy);
   }

   // get offset of scroller
   __with(i32 vofs = 0;) // offset in pixels of the content
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
   ifauto(str, cap, Lith_GUI_Prefix1(g, pre, capS))
      PrintSprite(cap, x,2, y,1);
   y += caph;

   // draw middle of bar
   __with(str scrl = Lith_GUI_Prefix1(g, pre, scrl);)
      for(i32 i = 0; i < blocks; i++)
   {
      if(scrl) PrintSprite(scrl, x,2, y,1);
      y += blockh;
   }

   // draw bottom cap
   ifauto(str, cap, Lith_GUI_Prefix1(g, pre, capE))
      PrintSprite(cap, x,2, y,1);

   // get base Y
   i32 const ory = a->y + g->oy;

   // draw scroller
   __with(str graphic;)
   {
      if(g->hot == id || g->active == id)
         graphic = Lith_GUI_Prefix1(g, pre, notchhot);
      else
         graphic = Lith_GUI_Prefix1(g, pre, notchgfx);

      if(graphic) for(i32 i = 0; i < notches; i++)
      {
         i32 const npos = roundlk(caph + h * scr->y + caph * i, 10);
         PrintSprite(graphic, x,2, ory + npos,1);
      }
   }

   // setup offsets
   g->ox += scr->ox;
   g->oy += scr->oy;

   // setup clip
   Lith_GUI_Clip(g, x, ory, a->w, realh, a->ww);
}

void Lith_GUI_ScrollEnd(struct gui_state *g, struct gui_scr *scr)
{
   // reset offsets
   g->ox -= scr->ox;
   g->oy -= scr->oy;

   // reset clip
   Lith_GUI_ClipRelease(g);
}

bool Lith_GUI_ScrollOcclude(struct gui_state *g, struct gui_scr const *scr, i32 y, i32 h)
{
   return y > scr->occludeE || (h && (y + h) - scr->occludeS < 0);
}

// EOF
