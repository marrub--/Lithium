#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"

#define GenPreset(type, def) \
   type const *pre; \
   if(a->preset) pre = a->preset; \
   else          pre = &def


//----------------------------------------------------------------------------
// Extern Objects
//

gui_button_preset_t const btndefault = {
   .gfx  = "lgfx/UI/Button.png",
   .hot  = "lgfx/UI/ButtonHot.png",
   .snd  = "player/cbi/buttonpress",
   .cdef = 'j',
   .cact = 'g',
   .chot = 'k',
   .cdis = 'u',
   .font = "CBIFONT",
   .w = 48,
   .h = 16
};

gui_button_preset_t const btntab = {
   .gfx  = "lgfx/UI/Tab.png",
   .hot  = "lgfx/UI/TabHot.png",
   .snd  = "player/cbi/buttonpress",
   .cdef = 'j',
   .cact = 'g',
   .chot = 'k',
   .cdis = 'n',
   .font = "CBIFONT",
   .w = 48,
   .h = 11
};

gui_button_preset_t const btnexit = {
   .gfx  = "lgfx/UI/ExitButton.png",
   .hot  = "lgfx/UI/ExitButtonHot.png",
   .snd  = "player/cbi/buttonpress",
   .w = 11,
   .h = 11
};

gui_button_preset_t const btnlist = {
   .gfx  = "lgfx/UI/ListButton.png",
   .hot  = "lgfx/UI/ListButtonHot.png",
   .snd  = "player/cbi/buttonpress",
   .cdef = 'j',
   .cact = 'g',
   .chot = 'k',
   .cdis = 'u',
   .font = "CBIFONT",
   .w = 80,
   .h = 9
};

gui_button_preset_t const btnbipmain = {
   .snd  = "player/cbi/buttonpress",
   .cdef = 'j',
   .cact = 'g',
   .chot = 'k',
   .cdis = 'n',
   .font = "CBIFONT",
   .w = 180,
   .h = 9
};

gui_button_preset_t const btnbipback = {
   .snd  = "player/cbi/buttonpress",
   .cdef = 'j',
   .cact = 'g',
   .chot = 'k',
   .cdis = 'n',
   .font = "CBIFONT",
   .w = 48,
   .h = 9
};

gui_scroll_preset_t const scrdefault = {
   .capS = "lgfx/UI/ListCapTop.png",
   .capE = "lgfx/UI/ListCapBottom.png",
   .scrl = "lgfx/UI/ListScrollbar.png",
   .scrlw = 8,
   .scrlh = 8,
   .notchgfx = "lgfx/UI/ListScrollNotch.png",
   .notchhot = "lgfx/UI/ListScrollNotchHot.png",
   .notchofs = 3
};


//----------------------------------------------------------------------------
// Static Functions
//

//
// GUI_Auto
//
void GUI_Auto(gui_state_t *g, id_t id, int x, int y, int w, int h)
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


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_GUI_Init
//
void Lith_GUI_Init(gui_state_t *g, size_t maxst)
{
   if(g->st) free(g->st);
   g->st = calloc(maxst, sizeof(gui_stateitem_t));
}

//
// Lith_GUI_UpdateState
//
void Lith_GUI_UpdateState(gui_state_t *g, player_t *p)
{
   bool inverted = ACS_GetUserCVar(p->number, "lith_player_invertmouse");
   
   // Due to ZDoom being ZDoom, GetUserCVar with invertmouse does nothing.
   // This breaks network sync so we can only do it in singleplayer.
   if(ACS_GameType() == GAME_SINGLE_PLAYER)
      inverted |= ACS_GetCVar("invertmouse");
   
   g->old = g->cur;
   
                g->cx -= p->yawv   * 800.0f;
   if(inverted) g->cy += p->pitchv * 800.0f;
   else         g->cy -= p->pitchv * 800.0f;
   
   g->cx = minmax(g->cx, 0, 320);
   g->cy = minmax(g->cy, 0, 200);
   
   g->clicklft = p->buttons & BT_ATTACK;
   g->clickrgt = p->buttons & BT_ALTATTACK;
   g->clickany = g->clicklft || g->clickrgt;
}

//
// Lith_GUI_Begin
//
void Lith_GUI_Begin(gui_state_t *g)
{
   g->hid = hid_end_cbi;
   g->hot = 0;
   
   ACS_SetHudSize(320, 200);
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
   
   ACS_SetHudClipRect(g->clpxS = x, g->clpyS = y, w, h);
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
// Lith_GUI_Button_Impl
//
bool Lith_GUI_Button_Impl(gui_state_t *g, id_t id, gui_button_args_t *a)
{
   GenPreset(gui_button_preset_t, btndefault);
   
   if(!a->disabled)
      GUI_Auto(g, id, a->x, a->y, pre->w, pre->h);
   
   {
   __str graphic;
   
   if(g->hot == id && pre->hot) graphic = pre->hot;
   else                         graphic = pre->gfx;
   
   if(graphic)
      DrawSpritePlain(graphic, g->hid--, a->x + g->ox + 0.1, a->y + g->oy + 0.1, TICSECOND);
   }
   
   if(a->label)
   {
      char color;
      
           if(a->disabled)     color = pre->cdis;
      else if(g->active == id) color = pre->cact;
      else if(g->hot    == id) color = pre->chot;
      else                     color = pre->cdef;
      
      HudMessageF(pre->font, "\C%c%S", color, a->label);
      HudMessagePlain(g->hid--, (pre->w / 2) + a->x + g->ox + 0.4, (pre->h / 2) + a->y + g->oy, TICSECOND);
   }
   
   if(g->hot == id && g->active == id && !g->clicklft)
   {
      ACS_LocalAmbientSound(pre->snd, 127);
      return true;
   }
   else
      return false;
}

//
// Lith_GUI_ScrollBegin_Impl
//
void Lith_GUI_ScrollBegin_Impl(gui_state_t *g, id_t id, gui_scroll_args_t *a)
{
   GenPreset(gui_scroll_preset_t, scrdefault);
   
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
   int x   = a->x + pre->scrlw + g->ox; // base x to draw from
   int y   = a->y              + g->oy; // base y to draw from
   int ory = y;                         // copy of y since it'll be changed later
   
   // check collision - height is minus caps, and y is offset by the top cap
   GUI_Auto(g, id, x - pre->scrlw, y + caph, pre->scrlw, h);
   
   // move scroll notch
   if(g->active == id)
   {
      // needs to be two expressions because minmax copies the expression
      scr->y = (g->cy - y) / (float)h;
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
   DrawSpritePlain(pre->notchgfx, g->hid--, x + 0.2, ory + (h * scr->y) + caph + 0.1, TICSECOND);
   
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
   gui_scroll_state_t *scr = &g->st[st].scrl;
   
   return
      y > scr->occludeE ||                // too low to be seen
      (h && (y + h) - scr->occludeS < 0); // too high to be seen (if height is available)
}

// EOF

