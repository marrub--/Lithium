// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Miscellaneous GUI functions.                                             │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"

noinit static struct gui_clip r;

stkoff static void G_cutBox(struct gui_clip const *other, struct gui_clip *r) {
   r->x  = clampi(r->x, other->x, other->x + other->w);
   r->y  = clampi(r->y, other->y, other->y + other->h);
   r->w  = mini(r->x + r->w,  other->x + other->w)  - r->x;
   r->h  = mini(r->y + r->h,  other->y + other->h)  - r->y;
   r->ww = mini(r->x + r->ww, other->x + other->ww) - r->x;
}

stkoff static void G_setClip(struct gui_state *g) {
   if(g->clip >= 0) {
      struct gui_clip *clip = &g->clips[g->clip];
      #ifndef NDEBUG
      if(dbgflags(dbgf_gui)) {
         i32 x = clip->x, y = clip->y, w = clip->w, h = clip->h;
         PrintLine(x,     y,     x + w, y + h, 0xFF00FFFF);
         PrintLine(x,     y,     x + w, y,     0xFF00FFFF);
         PrintLine(x,     y + h, x + w, y + h, 0xFF00FFFF);
         PrintLine(x + w, y,     x + w, y + h, 0xFF00FFFF);
         PrintLine(x,     y,     x,     y + h, 0xFF00FFFF);
         PrintLine(x, y + h / 2, x + clip->ww, y + h / 2, 0xFFFF00FF);
      }
      #endif
      SetClipW(clip->x, clip->y, clip->w, clip->h, clip->ww);
   } else {
      ClearClip();
   }
}

void G_Auto(struct gui_state *g, gid_t id, i32 x_, i32 y_, i32 w_, i32 h_, bool slide) {
   r.x = x_ + g->ox; r.y = y_ + g->oy; r.w = w_; r.h = h_; r.ww = w_;
   if(g->clip >= 0) {
      G_cutBox(&g->clips[g->clip], &r);
   }
   if(aabb_point(r.x, r.y, r.w - 1, r.h - 1, g->cx, g->cy)) {
      g->hot = id;
      if(g->active == 0 && g->clicklft) {
         g->active = id;
      }
   }
   /* check slide state */
   if(slide && g->slide != id && g->active == id) {
      g->slide      = id;
      g->slidetime  = 1;
      g->slidecount = 0;
   }
   #ifndef NDEBUG
   if(dbgflags(dbgf_gui)) {
      PrintLine(r.x,       r.y,       r.x + r.w, r.y + r.h, 0xFFFF0000);
      PrintLine(r.x,       r.y,       r.x + r.w, r.y,       0xFFFF0000);
      PrintLine(r.x,       r.y,       r.x,       r.y + r.h, 0xFFFF0000);
      PrintLine(r.x,       r.y + r.h, r.x + r.w, r.y + r.h, 0xFFFF0000);
      PrintLine(r.x + r.w, r.y,       r.x + r.w, r.y + r.h, 0xFFFF0000);
   }
   #endif
}

void G_Begin(struct gui_state *g, i32 w, i32 h) {
   fastmemcpy(&g->old, &g->cur, sizeof(g->old));
   if(!w) w = 320;
   if(!h) h = 200;
   g->hot     = 0;
   g->clip    = -1;
   g->tooltip = snil;
   SetSize(g->w = w, g->h = h);
   bool inverted = CVarGetI(sc_invertmouse);
   k32  curspeed = CVarGetK(sc_gui_curspeed) * 800.0k;
                g->cx -= pl.yawv   * curspeed * (g->w / (k32)g->h);
   if(inverted) g->cy += pl.pitchv * curspeed;
   else         g->cy -= pl.pitchv * curspeed;
   g->cx = clamplk(g->cx, 0, g->w);
   g->cy = clamplk(g->cy, 0, g->h);
   g->clicklft = P_ButtonHeld(BT_ATTACK);
   g->clickrgt = P_ButtonHeld(BT_ALTATTACK);
   g->clickany = g->clicklft || g->clickrgt;
   g->defcr = Draw_GetCr(CVarGetI(sc_gui_defcr));
   if(!g->clickany) {
      g->slide = 0;
   } else if(g->slidecount) {
      g->slidetime++;
   }
}

void G_End(struct gui_state *g, i32 curs) {
   if(g->tooltip) {
      noinit static struct i32v2 p, s;
      i32 ww;
      p.x = g->cx + 7;
      p.y = g->cy + 2;
      ww = g->w - p.x;
      TextSize(&s, g->tooltip, sf_smallfnt, ww);
      if(s.x < 100) {
         ww = g->cx - 2;
         TextSize(&s, g->tooltip, sf_smallfnt, ww);
         p.x = ww - s.x;
      }
      PrintRect(p.x-1, p.y-1, s.x+2, s.y+2, 0xFFBA8CC6);
      PrintRect(p.x,   p.y,   s.x,   s.y,   0xFF1A141D);
      SetClipW(p.x, p.y, s.x, s.y, ww);
      PrintText_str(g->tooltip, sf_smallfnt, CR_WHITE, p.x,1, p.y,1);
      ClearClip();
   }

   str cgfx;

   switch(curs) {
   default:
   case gui_curs_green:       cgfx = sp_UI_Cursor;            break;
   case gui_curs_pink:        cgfx = sp_UI_CursorPink;        break;
   case gui_curs_blue:        cgfx = sp_UI_CursorBlue;        break;
   case gui_curs_orange:      cgfx = sp_UI_CursorOrange;      break;
   case gui_curs_red:         cgfx = sp_UI_CursorRed;         break;
   case gui_curs_white:       cgfx = sp_UI_CursorWhite;       break;
   case gui_curs_outline:     cgfx = sp_UI_CursorOutline;     break;
   case gui_curs_outline2:    cgfx = sp_UI_CursorOutline2;    break;
   case gui_curs_outlineinv:  cgfx = sp_UI_CursorOutlineInv;  break;
   case gui_curs_outline2inv: cgfx = sp_UI_CursorOutline2Inv; break;
   }

   PrintSprite(cgfx, g->cx,1, g->cy,1);

   if(!g->clickany) {
      g->active = 0;
   }
}

void G_Clip(struct gui_state *g, i32 x, i32 y, i32 w, i32 h, i32 ww) {
   struct gui_clip *clip, *other;
   if(g->clip >= 0) {
      other = &g->clips[g->clip];
   } else {
      other = nil;
   }
   clip = &g->clips[++g->clip];
   clip->x = x + g->ox; clip->y = y + g->oy; clip->w = w; clip->h = h;
   clip->ww = ww ? mini(ww, clip->w) : clip->w;
   if(other) {
      G_cutBox(other, clip);
   }
   G_setClip(g);
}

void G_ClipRelease(struct gui_state *g) {
   g->clip--;
   G_setClip(g);
}

stkoff
bool G_Filler(i32 x, i32 y, struct gui_fil *fil, bool held) {
   if(fil->cur > fil->tic) {
      fil->cur = 0;
      return true;
   }
   if(held) {
      ++fil->cur;
   } else if(fil->cur && ACS_Timer() & 3 == 0) {
      --fil->cur;
   }
   PrintSprite(sa_filler[(fil->cur * 8) / fil->tic], x,1, y,0);
   return false;
}

stkcall void G_Tooltip(struct gui_state *g, i32 x_, i32 y_, i32 w_, i32 h_, cstr id) {
   r.x = x_ + g->ox; r.y = y_ + g->oy; r.w = w_; r.h = h_; r.ww = w_;
   if(g->clip >= 0) {
      G_cutBox(&g->clips[g->clip], &r);
   }
   if(aabb_point(r.x, r.y, r.w - 1, r.h - 1, g->cx, g->cy)) {
      str tt = lang(strp(_l(LANG "TOOLTIP_"), _p(id)));
      if(tt) {
         g->tooltip = tt;
      }
   }
}

i32 G_Tabs(struct gui_state *g, mem_size_t *st, gtab_t const *tn, mem_size_t num, i32 yp) {
   i32 xp = 0;
   for(mem_size_t i = 0; i < num; i++) {
      if(G_Button_HId(g, xp + yp * 6, tn[i], gui_p.btntab.w * xp,
                      gui_p.btntab.h * yp, i == *st, Pre(btntab))) {
         *st = i;
      }
      if(i != num - 1) {
         if(xp == 5) {
            xp = 0;
            yp++;
         } else {
            xp++;
         }
      }
   }
   return yp;
}

/* EOF */
