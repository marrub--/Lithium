// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

// Extern Functions ----------------------------------------------------------|

//
// Lith_GUI_Auto
//
void Lith_GUI_Auto(gui_state_t *g, id_t id, int x, int y, int w, int h, bool slide)
{
   x += g->ox;
   y += g->oy;

   // check clip versus cursor (if clipping), then check control versus cursor
   if(!g->useclip || aabb(g->clpxS, g->clpyS, g->clpxE, g->clpyE, g->cx, g->cy))
      if(aabb(x, y, x + w, y + h, g->cx, g->cy))
   {
      g->hot = id;

      if(g->active == 0 && g->clicklft)
         g->active = id;
   }

   // check slide state
   if(slide && g->slide != id && g->active == id) {
      g->slide      = id;
      g->slidetime  = 1;
      g->slidecount = 0;
   }
}

//
// Lith_GUI_Init
//
void Lith_GUI_Init(gui_state_t *g, void *state)
{
   g->state = state;
   g->gfxprefix = "lgfx/UI/";
}

//
// Lith_GUI_UpdateState
//
void Lith_GUI_UpdateState(gui_state_t *g, player_t *p)
{
   bool inverted = p->getCVarI("lith_player_invertmouse");

   // Due to ZDoom being ZDoom, GetUserCVar with invertmouse does nothing.
   // This breaks network sync so we can only do it in singleplayer.
   if(world.singleplayer)
      inverted |= ACS_GetCVar("invertmouse");

   g->old = g->cur;

   fixed xmul = p->getCVarK("lith_gui_xmul");
   fixed ymul = p->getCVarK("lith_gui_ymul");

                g->cx -= p->yawv   * (800.0f * xmul);
   if(inverted) g->cy += p->pitchv * (800.0f * ymul);
   else         g->cy -= p->pitchv * (800.0f * ymul);

   g->cx = minmax(g->cx, 0, g->w);
   g->cy = minmax(g->cy, 0, g->h);

   g->clicklft = p->buttons & BT_ATTACK;
   g->clickrgt = p->buttons & BT_ALTATTACK;
   g->clickany = g->clicklft || g->clickrgt;

   if(!g->clickany)
      g->slide = 0;
   else if(g->slidecount)
      g->slidetime++;
}

//
// Lith_GUI_Begin
//
void Lith_GUI_Begin(gui_state_t *g, int w, int h)
{
   if(!w) w = 320;
   if(!h) h = 200;

   g->hot = 0;

   ACS_SetHudSize(g->w = w, g->h = h);
   SetSize(g->w, g->h);
}

//
// Lith_GUI_End
//
void Lith_GUI_End(gui_state_t *g, enum cursor curs)
{
   __str cgfx;

   switch(curs)
   {
   default:
   case gui_curs_green:       cgfx = "lgfx/UI/Cursor.png";            break;
   case gui_curs_pink:        cgfx = "lgfx/UI/CursorPink.png";        break;
   case gui_curs_blue:        cgfx = "lgfx/UI/CursorBlue.png";        break;
   case gui_curs_orange:      cgfx = "lgfx/UI/CursorOrange.png";      break;
   case gui_curs_red:         cgfx = "lgfx/UI/CursorRed.png";         break;
   case gui_curs_white:       cgfx = "lgfx/UI/CursorWhite.png";       break;
   case gui_curs_outline:     cgfx = "lgfx/UI/CursorOutline.png";     break;
   case gui_curs_outline2:    cgfx = "lgfx/UI/CursorOutline2.png";    break;
   case gui_curs_outlineinv:  cgfx = "lgfx/UI/CursorOutlineInv.png";  break;
   case gui_curs_outline2inv: cgfx = "lgfx/UI/CursorOutline2Inv.png"; break;
   }

   PrintSprite(cgfx, g->cx,1, g->cy,1);

   if(!g->clickany)
      g->active = 0;
}

//
// Lith_GUI_Clip
//
void Lith_GUI_Clip(gui_state_t *g, int x, int y, int w, int h, int ww)
{
   g->useclip = true;
   g->clpxE = x + w;
   g->clpyE = y + h;

   if(ww == 0) ww = w;
   SetClipW(g->clpxS = x, g->clpyS = y, w, h, ww);
}

//
// Lith_GUI_ClipRelease
//
void Lith_GUI_ClipRelease(gui_state_t *g)
{
   g->useclip = g->clpxS = g->clpyS = g->clpxE = g->clpyE = 0;
   ClearClip();
}

//
// Lith_GUI_TypeOn
//
void Lith_GUI_TypeOn(gui_state_t *g, gui_typeon_state_t *typeon, __str text)
{
   typeon->txt = text;
   typeon->len = ACS_StrLen(text);
   typeon->pos = 0;
}

//
// Lith_RemoveTextColors
//
__str Lith_RemoveTextColors(__str str, int size)
{
   [[__no_init]]
   static char buf[8192];
   int j = 0;

   if(size > countof(buf)) return "[programmer error, please report]";

   for(int i = 0; i < size; i++)
   {
      if(str[i] == '\C')
      {
         i++;
         if(str[i] == '[')
            while(str[i] && str[i++] != ']');
         else
            i++;
      }

      if(i >= size || j >= size || !str[i])
         break;

      buf[j++] = str[i];
   }

   return StrParam("%.*s", j, buf);
}

//
// Lith_GUI_TypeOnUpdate
//
gui_typeon_state_t const *Lith_GUI_TypeOnUpdate(gui_state_t *g, gui_typeon_state_t *typeon)
{
   int num = ACS_Random(2, 15);

   if((typeon->pos += num) > typeon->len)
      typeon->pos = typeon->len;

   return typeon;
}

// EOF

