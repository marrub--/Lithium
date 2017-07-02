#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_GUI_Auto
//
void Lith_GUI_Auto(gui_state_t *g, id_t id, int x, int y, int w, int h)
{
   x += g->ox;
   y += g->oy;
   
   // check clip versus cursor (if clipping), then check control versus cursor
   if(!g->useclip || bpcldi(g->clpxS, g->clpyS, g->clpxE, g->clpyE, g->cx, g->cy))
      if(bpcldi(x, y, x + w, y + h, g->cx, g->cy))
   {
      g->hot = id;
      
      if(g->active == 0 && g->clicklft)
         g->active = id;
   }
}

//
// Lith_GUI_Init
//
void Lith_GUI_Init(gui_state_t *g, size_t maxst)
{
   if(g->st) free(g->st);
   if(maxst) g->st = calloc(maxst, sizeof(gui_stateitem_t));
   else      g->st = null;
   g->gfxprefix = "lgfx/UI/";
}

//
// Lith_GUI_UpdateState
//
void Lith_GUI_UpdateState(gui_state_t *g, player_t *p)
{
   bool inverted = Lith_GetPCVarInt(p, "lith_player_invertmouse");
   
   // Due to ZDoom being ZDoom, GetUserCVar with invertmouse does nothing.
   // This breaks network sync so we can only do it in singleplayer.
   if(world.singleplayer)
      inverted |= ACS_GetCVar("invertmouse");
   
   g->old = g->cur;
   
   fixed xmul = Lith_GetPCVarFixed(p, "lith_gui_xmul");
   fixed ymul = Lith_GetPCVarFixed(p, "lith_gui_ymul");
   
                g->cx -= p->yawv   * (800.0f * xmul);
   if(inverted) g->cy += p->pitchv * (800.0f * ymul);
   else         g->cy -= p->pitchv * (800.0f * ymul);
   
   g->cx = minmax(g->cx, 0, g->w);
   g->cy = minmax(g->cy, 0, g->h);
   
   g->clicklft = p->buttons & BT_ATTACK;
   g->clickrgt = p->buttons & BT_ALTATTACK;
   g->clickany = g->clicklft || g->clickrgt;
}

//
// Lith_GUI_Begin
//
void Lith_GUI_Begin(gui_state_t *g, int basehid, int w, int h)
{
   if(!w) w = 320;
   if(!h) h = 200;
   
   g->hid = basehid;
   g->hot = 0;
   
   ACS_SetHudSize(g->w = w, g->h = h);
}

//
// Lith_GUI_End
//
void Lith_GUI_End(gui_state_t *g)
{
   DrawSpritePlain("lgfx/UI/Cursor.png", g->hid--, (int)g->cx + 0.1, (int)g->cy + 0.1, TICSECOND);
   
   if(!g->clickany)
      g->active = 0;
}

//
// Lith_GUI_Clip
//
void Lith_GUI_Clip(gui_state_t *g, int x, int y, int w, int h)
{
   g->useclip = true;
   g->clpxE = x + w;
   g->clpyE = y + h;
   
   ACS_SetHudClipRect(g->clpxS = x, g->clpyS = y, w, h, w);
}

//
// Lith_GUI_ClipRelease
//
void Lith_GUI_ClipRelease(gui_state_t *g)
{
   g->useclip = g->clpxS = g->clpyS = g->clpxE = g->clpyE = 0;
   ACS_SetHudClipRect(0, 0, 0, 0);
}

//
// Lith_GUI_TypeOn
//
void Lith_GUI_TypeOn(gui_state_t *g, size_t st, __str text)
{
   gui_typeon_state_t *typeon = &g->st[st].type;
   
   typeon->txt = text;
   typeon->len = ACS_StrLen(text);
   typeon->pos = 0;
}

//
// Lith_GUI_TypeOnUpdate
//
gui_typeon_state_t const *Lith_GUI_TypeOnUpdate(gui_state_t *g, size_t st)
{
   gui_typeon_state_t *typeon = &g->st[st].type;
   if((typeon->pos += ACS_Random(2, 15)) > typeon->len) typeon->pos = typeon->len;
   return typeon;
}

// EOF

