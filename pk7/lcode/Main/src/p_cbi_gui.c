#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"

#define GenPreset(type, def) \
   type const *pre; \
   if(a->preset) pre = a->preset; \
   else          pre = &def

#define BtnDefault \
   .snd  = "player/cbi/buttonpress", \
   .cdef = "j", \
   .cact = "g", \
   .chot = "k", \
   .font = "CBIFONT"


//----------------------------------------------------------------------------
// Extern Objects
//

gui_button_preset_t const btndefault = {
   BtnDefault,
   .gfx  = "lgfx/UI/Button.png",
   .hot  = "lgfx/UI/ButtonHot.png",
   .cdis = "m",
   .w = 48,
   .h = 16
};

gui_button_preset_t const btntab = {
   BtnDefault,
   .gfx  = "lgfx/UI/Tab.png",
   .hot  = "lgfx/UI/TabHot.png",
   .cdis = "i",
   .w = 46,
   .h = 11
};

gui_button_preset_t const btnexit = {
   .gfx  = "lgfx/UI/ExitButton.png",
   .hot  = "lgfx/UI/ExitButtonHot.png",
   .w = 11,
   .h = 11
};

gui_button_preset_t const btnnext = {
   BtnDefault,
   .gfx  = "lgfx/UI/BtnNext.png",
   .hot  = "lgfx/UI/BtnNextHot.png",
   .w = 8,
   .h = 11
};

gui_button_preset_t const btnprev = {
   BtnDefault,
   .gfx  = "lgfx/UI/BtnPrev.png",
   .hot  = "lgfx/UI/BtnPrevHot.png",
   .w = 8,
   .h = 11
};

gui_button_preset_t const btnlist = {
   BtnDefault,
   .gfx  = "lgfx/UI/ListButton.png",
   .hot  = "lgfx/UI/ListButtonHot.png",
   .cdis = "u",
   .w = 80,
   .h = 9
};

gui_button_preset_t const btnlistsel = {
   BtnDefault,
   .gfx  = "lgfx/UI/ListButton.png",
   .hot  = "lgfx/UI/ListButtonHot.png",
   .cdis = "i",
   .w = 80,
   .h = 9
};

gui_button_preset_t const btnlistactive = {
   BtnDefault,
   .gfx  = "lgfx/UI/ListButtonActive.png",
   .hot  = "lgfx/UI/ListButtonActiveHot.png",
   .cdis = "i",
   .w = 80,
   .h = 9
};

gui_button_preset_t const btnlistactivated = {
   BtnDefault,
   .gfx  = "lgfx/UI/ListButtonActivated.png",
   .hot  = "lgfx/UI/ListButtonActiveHot.png",
   .cdis = "i",
   .w = 80,
   .h = 9
};

gui_button_preset_t const btnbipmain = {
   BtnDefault,
   .cdis = "n",
   .w = 180,
   .h = 9
};

gui_button_preset_t const btnbipback = {
   BtnDefault,
   .cdis = "n",
   .w = 48,
   .h = 9
};

gui_checkb_preset_t const cbxdefault = {
   .gfx    = "lgfx/UI/Checkbox.png",
   .hot    = "lgfx/UI/CheckboxHot.png",
   .sndup  = "player/cbi/clickon",
   .snddn  = "player/cbi/clickoff",
   .chkgfx = "lgfx/UI/CheckboxX.png",
   .chkhot = "lgfx/UI/CheckboxXHot.png",
   .chkact = "lgfx/UI/CheckboxXActive.png",
   .font = "CNFONT",
   .w = 10,
   .h = 10
};

gui_scroll_preset_t const scrdefault = {
   .capS  = "lgfx/UI/ListCapTop.png",
   .capE  = "lgfx/UI/ListCapBottom.png",
   .scrl  = "lgfx/UI/ListScrollbar.png",
   .scrlw = 8,
   .scrlh = 8,
   .notchgfx = "lgfx/UI/ListScrollNotch.png",
   .notchhot = "lgfx/UI/ListScrollNotchHot.png"
};

gui_slider_preset_t const slddefault = {
   .gfx    = "lgfx/UI/Slider.png",
   .snd    = "player/cbi/slidertick",
   .notch  = "lgfx/UI/SliderNotch.png",
   .notchhot = "lgfx/UI/SliderNotchHot.png",
   .pad = 2,
   .w   = 80,
   .h = 7
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
   
   g->cx = minmax(g->cx, 0, g->w);
   g->cy = minmax(g->cy, 0, g->h);
   
   g->clicklft = p->buttons & BT_ATTACK;
   g->clickrgt = p->buttons & BT_ALTATTACK;
   g->clickany = g->clicklft || g->clickrgt;
}

//
// Lith_GUI_Begin
//
void Lith_GUI_Begin(gui_state_t *g, int w, int h)
{
   if(!w) w = 320;
   if(!h) h = 200;
   
   g->hid = hid_end_cbi;
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
      __str color;
      
           if(a->disabled)     color = pre->cdis;
      else if(g->active == id) color = pre->cact;
      else if(g->hot    == id) color = pre->chot;
      else if(a->color)        color = a->color;
      else                     color = pre->cdef;
      
      HudMessageF(pre->font, "\C%S%S", color, a->label);
      HudMessagePlain(g->hid--, (pre->w / 2) + a->x + g->ox + 0.4, (pre->h / 2) + a->y + g->oy, TICSECOND);
   }
   
   if(g->hot == id && g->active == id && !g->clicklft)
   {
      if(pre->snd) ACS_LocalAmbientSound(pre->snd, 127);
      return true;
   }
   else return false;
}

//
// Lith_GUI_Checkbox_Impl
//
bool Lith_GUI_Checkbox_Impl(gui_state_t *g, id_t id, gui_checkb_args_t *a)
{
   GenPreset(gui_checkb_preset_t, cbxdefault);
   
   if(!a->disabled)
      GUI_Auto(g, id, a->x - (pre->w/2), a->y - (pre->h/2), pre->w, pre->h);
   
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
   int x = a->x + pre->scrlw; // base x to draw from
   int y = a->y;              // base y to draw from
   
   // check collision - height is minus caps, and y is offset by the top cap
   GUI_Auto(g, id, x - pre->scrlw, y + caph, pre->scrlw, h);
   
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

//
// Lith_GUI_Slider_Impl
//
double Lith_GUI_Slider_Impl(gui_state_t *g, id_t id, gui_slider_args_t *a)
{
   GenPreset(gui_slider_preset_t, slddefault);
   
   double w = pre->w - (pre->pad * 2);
   
   int x = a->x + pre->pad;
   int y = a->y;
   
   GUI_Auto(g, id, x, y, w, pre->h);
   
   x += g->ox;
   y += g->oy;
   
   // get a normalized value
   double aval;
   
   aval = (a->val - a->minima) / (a->maxima - a->minima);
   aval = minmax(aval, 0, 1);
   
   double val;
   
   // move scroll notch
   if(g->active == id)
   {
      val  = g->cx - x;
      val  = minmax(val, 0, w);
      val /= w;
      
      // play sound
      if(g->cx != g->old.cx && g->cx >= x && g->cx < x + w)
         ACS_LocalAmbientSound(pre->snd, 60);
   }
   else
      val = aval;
   
   // get result-normalized value
   double norm = val * (a->maxima - a->minima) + a->minima;
   
   // draw graphic
   DrawSpritePlain(pre->gfx, g->hid--, (x - pre->pad) + 0.1, y + (pre->h / 2), TICSECOND);
   
   // draw notch
   {
   __str graphic = g->hot == id || g->active == id ? pre->notchhot : pre->notch;
   DrawSpritePlain(graphic, g->hid--, x + (int)(val * w) - 1 + 0.1, y + 0.1, TICSECOND);
   }
   
   // draw value
   HudMessageF("CBIFONT", "\Cj%.1k", (fixed)(norm + 0.001));
   HudMessagePlain(g->hid--, x + (pre->w / 2) + 0.4, y + (pre->h / 2), TICSECOND);
   
   // if we've moved it, we return a difference
   if(g->active == id && !g->clicklft && !IsSmallNumber(aval - val))
      return norm - a->val;
   
   // otherwise we return 0
   else
      return 0;
}

// EOF

