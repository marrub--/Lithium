// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ GUI scroll bars.                                                         │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"

/* This function is excessively commented in case I need to change it.
 * Reading this function will cause a 5x1d20 hit to your SAN stat. Beware!
 */
void G_ScrBeg_Imp(struct gui_state *g, gid_t id, struct gui_arg_scr *a) {
   struct gui_pre_scr const *pre = a->preset |? &gui_p.scrdef;
   struct gui_scr *scr = a->st;

   /* sizes */
   i32 const blockh = pre->scrlh;        /* height of graphical block */
   i32 const blocks = a->h / blockh - 1; /* height in graphical blocks -caps */
   i32 const caph   = blockh / 2;        /* size of cap */
   i32 const caps   = blocks * 2;        /* height in caps, -caps */
   i32 const h      = blocks * blockh;   /* height in pixels -caps */
   i32 const realh  = h + caph * 2;      /* height in pixels +caps */

   /* positions */
   i32 x = a->x + pre->scrlw; /* base x to draw from */
   i32 y = a->y;              /* base y to draw from */

   /* check collision - height is minus caps, and y is offset by the top cap */
   G_Auto(g, id, x - pre->scrlw, y + caph, pre->scrlw, h);

   /* add offset */
   x += g->ox;
   y += g->oy;

   /* get height of scroller */
   i32 notches; /* height of scroller in caps */

   if(a->contenth > realh) notches = a->h / (k64)a->contenth * caps;
   else                    notches = caps;

   i32 const scrlh = notches * caph;       /* height in pixels of scroller */
   k64 const maxy  = (h - scrlh) / (k64)h; /* normalized maximum y value */

   /* decrement the sound timer */
   if(scr->nextsnd) scr->nextsnd--;

   /* move scroller */
   __with(k64 supposedy = scr->y * h;)
   {
      if(g->active == id)
      {
         k64 const cy = g->cy - y - caph;

         /* if it isn't grabbed and the cursor is over the scroller, */
         /* set the grab position to where the cursor is relative to it */
         if(!scr->grabbed && cy > supposedy && cy < supposedy + scrlh)
         {
            scr->grabbed = true;
            scr->grabpos = cy - supposedy;
         }

         /* if the scroller is grabbed we set the position relative to where */
         /* we grabbed it from, otherwise we just use the middle of it */
         if(scr->grabbed) supposedy = cy - scr->grabpos;
         else             supposedy = cy - scrlh / 2;

         if(g->cy != g->old.cy && scr->nextsnd == 0)
         {
            AmbientSound(ss_player_cbi_scroll, 1.0);
            scr->nextsnd = 7;
         }
      }
      else if(scr->grabbed)
      {
         AmbientSound(ss_player_cbi_scrollend, 1.0);
         scr->grabbed = false;
         scr->nextsnd = 0;
      }

      /* finally, normalize and clamp */
      scr->y = supposedy / (k64)h;
      scr->y = clamplk(scr->y, 0, maxy);
   }

   /* get offset of scroller */
   __with(i32 vofs = 0;) /* offset in pixels of the content */
   {
      if(a->contenth > realh && maxy)
         vofs = roundlk((a->contenth - realh) * (scr->y / maxy), 10);

      /* set the scrollbar's offset */
      scr->ox = a->x + pre->scrlw; /* offset by scrollbar width */
      scr->oy = a->y - vofs;       /* offset by scroller pos */

      /* set the top and bottom for occlusion */
      scr->occludeS = vofs;
      scr->occludeE = vofs + realh;
   }

   /* draw top cap */
   __with(char cap[64];)
   {
      G_Prefix(g, cap, pre, capS);
      PrintSprite(fast_strdup(cap), x,2, y,1);
   }
   y += caph;

   /* draw middle of bar */
   __with(char scrl[64];)
   {
      G_Prefix(g, scrl, pre, scrl);
      str scrls = fast_strdup(scrl);
      for(i32 i = 0; i < blocks; i++)
      {
         PrintSprite(scrls, x,2, y,1);
         y += blockh;
      }
   }

   /* draw bottom cap */
   __with(char cap[64];)
   {
      G_Prefix(g, cap, pre, capE);
      if(cap[0]) PrintSprite(fast_strdup(cap), x,2, y,1);
   }

   /* get base Y */
   i32 const ory = a->y + g->oy;

   /* draw scroller */
   __with(char gfx[64];)
   {
      if(g->hot == id || g->active == id) G_Prefix(g, gfx, pre, notchhot);
      else                                G_Prefix(g, gfx, pre, notchgfx);

      str gfxs = fast_strdup(gfx);
      for(i32 i = 0; i < notches; i++)
      {
         i32 const npos = roundlk(caph + h * scr->y + caph * i, 10);
         PrintSprite(gfxs, x,2, ory + npos,1);
      }
   }

   /* setup offsets */
   g->ox += scr->ox;
   g->oy += scr->oy;

   /* setup clip */
   G_Clip(g, x, ory, a->w, realh, a->ww);
}

void G_ScrEnd(struct gui_state *g, struct gui_scr *scr) {
   /* reset offsets */
   g->ox -= scr->ox;
   g->oy -= scr->oy;

   /* reset clip */
   G_ClipRelease(g);
}

bool G_ScrOcc(struct gui_state *g, struct gui_scr const *scr, i32 y, i32 h) {
   return y > scr->occludeE || (h && (y + h) - scr->occludeS < 0);
}

/* EOF */
