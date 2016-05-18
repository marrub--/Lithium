#include "lith_common.h"
#include "lith_cbi.h"

//
// GUI_Auto
//
// Offset x/y coordinates and check for clipping.
// Check if mouse is colliding with us, and if it's being clicked.
//

static
void GUI_Auto(int id, bool active, gui_state_t *gst, int *x, int *y, int dim_x, int dim_y, int clickmask)
{
   struct gui_clip_s *clip = &gst->clip;
   
   *x += gst->ofsx;
   *y += gst->ofsy;
   
   if(active &&
      bpcldi(*x, *y, *x + dim_x, *y + dim_y, gst->cur.x, gst->cur.y) &&
      (!clip->on || bpcldi(clip->x, clip->y, clip->x + clip->w, clip->y + clip->h, gst->cur.x, gst->cur.y)))
   {
      gst->hot = id;
      
      if(gst->active == GUI_EMPTY_ID && gst->cur.click & clickmask)
         gst->active = id;
   }
}

#define DEFAULT(n) if(!parm.n) parm.n = parm_default.n;

//
// GUI_Button
//

[[__optional_args(3)]]
bool GUI_Button(int id, gui_state_t *gst, int *hid, int x, int y, __str text, bool inactive, gui_button_parm_t const *parm_)
{
   static gui_button_parm_t parm_default = {
      .clickmask = GUI_CLICK_LEFT,
      .c_default = 'j',
      .c_active = 'g',
      .c_hot = 'k',
      .c_inactive = 'u',
      .f_font = "CBIFONT",
      .f_gfx_def = "lgfx/UI/Button.png",
      .f_gfx_hot = "lgfx/UI/ButtonHot.png",
      .s_clicked = "player/cbi/buttonpress",
      .dim_x = GUI_BUTTON_W,
      .dim_y = GUI_BUTTON_H,
   };
   
   static gui_button_parm_t parm;
   
   //
   // Set up structured parameters.
   
   if(parm_)
   {
      parm = *parm_;
      DEFAULT(clickmask);
      DEFAULT(c_default); DEFAULT(c_active); DEFAULT(c_hot); DEFAULT(c_inactive);
      DEFAULT(f_font); DEFAULT(f_gfx_def); DEFAULT(f_gfx_hot);
      DEFAULT(s_clicked);
      DEFAULT(dim_x); DEFAULT(dim_y);
   }
   else
      parm = parm_default;
   
   GUI_Auto(id, !inactive, gst, &x, &y, parm.dim_x, parm.dim_y, parm.clickmask);
   
   //
   // Draw button.
   
   if(gst->hot == id)
      DrawSpritePlain(parm.f_gfx_hot, (*hid)--, 0.1 + x, 0.1 + y, TICSECOND);
   else
      DrawSpritePlain(parm.f_gfx_def, (*hid)--, 0.1 + x, 0.1 + y, TICSECOND);
   
   //
   // [opt] Draw text.
   
   if(text)
   {
      char color = parm.c_default;
      
      if(inactive)
         color = parm.c_inactive;
      else if(gst->active == id)
         color = parm.c_active;
      else if(gst->hot == id)
         color = parm.c_hot;
      
      HudMessageF(parm.f_font, "\C%c%S", color, text);
      HudMessagePlain((*hid)--, (parm.dim_x/2) + x, (parm.dim_y/2) + y, TICSECOND);
   }
   
   //
   // Check if click ended. If so, play a sound and return true.
   
   if(!(gst->cur.click & parm.clickmask) && gst->hot == id && gst->active == id)
   {
      ACS_LocalAmbientSound(parm.s_clicked, 127);
      return true;
   }
   else
      return false;
}

//
// GUI_ScrollBarVertical
//

static
bool GUI_ScrollBarVertical(int id, gui_state_t *gst, int *hid, int x, int y, int size, int *pos, gui_scroll_parm_t const *parm)
{
   // REAL THINGS
   int height = size / parm->dim_scrl_y;
   int realheight = height * parm->dim_scrl_y; // yay, integer rounding abuse!
   
   // MAGICAL THINGS
   int capheight = realheight - (parm->dim_cap_y * 2);
   int ypos = (parm->dim_cap_y + capheight * *pos) / 65536;
   int capy = y + parm->dim_cap_y;
   
   // CODE THINGS
   GUI_Auto(id, true, gst, &x, &capy, parm->dim_scrl_x, capheight, parm->clickmask);
   
   //
   // Draw each section of the scroll bar
   
   for(int i = 0; i < height; i++)
      DrawSpritePlain(parm->f_gfx_scrl, (*hid)--, 0.1 + x, 0.1 + y + (parm->dim_scrl_y * i), TICSECOND);
   
   DrawSpritePlain(parm->f_gfx_capS, (*hid)--, 0.1 + x, 0.1 + y, TICSECOND);
   DrawSpritePlain(parm->f_gfx_capE, (*hid)--, 0.1 + x, 0.1 + y + ((parm->dim_scrl_y * height) - parm->dim_cap_y), TICSECOND);
   
   //
   // Draw the notch thingy
   
   if(gst->active == id || gst->hot == id)
      DrawSpritePlain(parm->f_gfx_notch_hot, (*hid)--, 0.1 + x - parm->ofs_notch, 0.1 + capy + ypos, TICSECOND);
   else
      DrawSpritePlain(parm->f_gfx_notch, (*hid)--, 0.1 + x - parm->ofs_notch, 0.1 + capy + ypos, TICSECOND);
   
   //
   // Check for input.
   
   if(gst->active == id)
   {
      int mpos = minmax(gst->cur.y - capy, 0, capheight);
      int v = (mpos * 65536) / capheight;
      if(*pos != v)
      {
         *pos = v;
         return true;
      }
   }
   
   return false;
}

//
// GUI_ScrollBar
//

[[__optional_args(1)]]
bool GUI_ScrollBar(int id, gui_state_t *gst, int *hid, int x, int y, int size, int *pos, gui_scroll_parm_t const *parm_)
{
   static gui_scroll_parm_t parm_default = {
      .clickmask = GUI_CLICK_LEFT,
      .vertical = false,
      // TODO: split these graphics
      .f_gfx_capS = "lgfx/UI/ScrollLeft.png",
      .f_gfx_capE = "lgfx/UI/ScrollRight.png",
      .f_gfx_scrl = "lgfx/UI/ScrollMid.png",
      .f_gfx_notch = "lgfx/UI/ScrollerNotch.png",
      .f_gfx_notch_hot = "lgfx/UI/ScrollerNotchHot.png",
      /* .dim_cap_x = GUI_SCRLCAP_W,
      .dim_cap_y = GUI_SCRLCAP_H,
      .dim_scrl_x = GUI_SCRL_W,
      .dim_scrl_y = GUI_SCRL_H,
      .ofs_notch = GUI_SCRL_NOTCH_OFS */
   };
   
   static gui_scroll_parm_t parm;
   
   //
   // Set up structured parameters.
   
   if(parm_)
   {
      parm = *parm_;
      DEFAULT(clickmask);
      DEFAULT(f_gfx_capS); DEFAULT(f_gfx_capE); DEFAULT(f_gfx_scrl);
      DEFAULT(f_gfx_notch);
      DEFAULT(dim_cap_x); DEFAULT(dim_cap_y); DEFAULT(dim_scrl_x); DEFAULT(dim_scrl_y);
   }
   else
      parm = parm_default;
   
   //
   // Do the thing.
   
   if(parm.vertical)
      return GUI_ScrollBarVertical(id, gst, hid, x, y, size, pos, &parm);
   else
   {
      // TODO
      // return GUI_ScrollBarHorizontal(id, gst, hid, x, y, size, pos, &parm);
      return false;
   }
}

#undef DEFAULT

//
// GUI_BeginOffset
//

void GUI_BeginOffset(gui_state_t *gst, int x, int y)
{
   gst->ofsx = x;
   gst->ofsy = y;
}

//
// GUI_EndOffset
//

void GUI_EndOffset(gui_state_t *gst)
{
   gst->ofsx = 0;
   gst->ofsy = 0;
}

//
// GUI_BeginClip
//

void GUI_BeginClip(gui_state_t *gst, int x, int y, int w, int h)
{
   if(gst->clip.on)
      return;
   
   gst->clip.on = true;
   
   gst->clip.x = x;
   gst->clip.y = y;
   gst->clip.w = w;
   gst->clip.h = h;
   
   ACS_SetHudClipRect(x, y, w, h);
}

//
// GUI_EndClip
//

void GUI_EndClip(gui_state_t *gst)
{
   if(!gst->clip.on)
      return;
   
   gst->clip.on = false;
   ACS_SetHudClipRect(0, 0, 0, 0);
}

//
// GUI_Begin
//

void GUI_Begin(gui_state_t *gst)
{
   ACS_SetHudSize(320, 200);
   
   gst->hot = GUI_EMPTY_ID;
}

//
// GUI_End
//

void GUI_End(gui_state_t *gst)
{
   if(gst->cur.click == GUI_CLICK_NONE)
      gst->active = GUI_EMPTY_ID;
   else if(gst->active == GUI_EMPTY_ID)
      gst->active = GUI_INVALID_ID;
   
   if(gst->clip.on)
      GUI_EndClip(gst);
   
   if(gst->ofsx || gst->ofsy)
      GUI_EndOffset(gst);
}

