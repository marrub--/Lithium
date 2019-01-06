// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// g_preset.c: GUI presets.

#include "common.h"
#include "p_player.h"

struct gui_presets const gui_p = {
   .btndef = {
      .gfx      = "Button",
      .hot      = "ButtonHot",
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "m",
      .font     = "cbifont",
      .external = false,
      .w        = 48,
      .h        = 16,
      .ax = 4, .ay = 0
   },

   .btntab = {
      .gfx      = "Tab",
      .hot      = "TabHot",
      .snd      = "player/cbi/tabswitch",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 46,
      .h        = 11,
      .ax = 4, .ay = 0
   },

   .btnexit = {
      .gfx      = "ExitButton",
      .hot      = "ExitButtonHot",
      .external = false,
      .w        = 11,
      .h        = 11,
      .ax = 4, .ay = 0
   },

   .btnnext = {
      .gfx      = "BtnNext",
      .hot      = "BtnNextHot",
      .snd      = "player/cbi/right",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 8,
      .h        = 11,
      .ax = 4, .ay = 0
   },

   .btnprev = {
      .gfx      = "BtnPrev",
      .hot      = "BtnPrevHot",
      .snd      = "player/cbi/left",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 8,
      .h        = 11,
      .ax = 4, .ay = 0
   },

   .btnnexts = {
      .gfx      = "BtnNextSmall",
      .hot      = "BtnNextSmallHot",
      .snd      = "player/cbi/buttoncurs",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 6,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnprevs = {
      .gfx      = "BtnPrevSmall",
      .hot      = "BtnPrevSmallHot",
      .snd      = "player/cbi/buttoncurs",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 6,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnlist = {
      .gfx      = "ListButton",
      .hot      = "ListButtonHot",
      .snd      = "player/cbi/clicklst",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "u",
      .font     = "cbifont",
      .external = false,
      .w        = 80,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnlistsel = {
      .gfx      = "ListButton",
      .hot      = "ListButtonHot",
      .snd      = "player/cbi/clicklst",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 80,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnlistactive = {
      .gfx      = "ListButtonActive",
      .hot      = "ListButtonActiveHot",
      .snd      = "player/cbi/clicklst",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 80,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnlistactivated = {
      .gfx      = "ListButtonActivated",
      .hot      = "ListButtonActiveHot",
      .snd      = "player/cbi/clicklst",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 80,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnbipmain = {
      .gfx      = nil,
      .hot      = nil,
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "-",
      .font     = "cbifont",
      .external = false,
      .w        = 180,
      .h        = 9,
      .ax = 1, .ay = 0
   },

   .btnbipback = {
      .gfx      = nil,
      .hot      = nil,
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "n",
      .font     = "cbifont",
      .external = false,
      .w        = 48,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btndlgsel = {
      .gfx      = ":Dialogue:SelectBack",
      .hot      = ":Dialogue:SelectBackHot",
      .snd      = "player/cbi/dlgclick",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "d",
      .cdis     = "i",
      .font     = "cbifont",
      .external = true,
      .w        = 240,
      .h        = 14,
      .ax = 4, .ay = 0
   },

   .btnclear = {
      .gfx      = nil,
      .hot      = nil,
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "n",
      .font     = "cbifont",
      .external = false,
      .w        = 20,
      .h        = 8,
      .ax = 1, .ay = 1
   },

   .btnnote = {
      .gfx      = nil,
      .hot      = nil,
      .snd      = "player/cbi/buttoncurs",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "-",
      .font     = "cbifont",
      .external = false,
      .w        = 200,
      .h        = 30,
      .ax = 1, .ay = 1
   },

   .cbxdef = {
      .gfx      = s"Checkbox",
      .hot      = s"CheckboxHot",
      .dis      = s"CheckboxDisabled",
      .sndup    = s"player/cbi/clickon",
      .snddn    = s"player/cbi/clickoff",
      .chkgfx   = s"CheckboxX",
      .chkhot   = s"CheckboxXHot",
      .chkact   = s"CheckboxXActive",
      .chkdis   = s"CheckboxXDisabled",
      .external = false,
      .w        = 10,
      .h        = 10
   },

   .cbxsmall = {
      .gfx      = s"CheckboxSmall",
      .hot      = s"CheckboxSmallHot",
      .dis      = s"CheckboxSmallDisabled",
      .sndup    = s"player/cbi/clickon",
      .snddn    = s"player/cbi/clickoff",
      .chkgfx   = s"CheckboxX",
      .chkhot   = s"CheckboxXHot",
      .chkact   = s"CheckboxXActive",
      .chkdis   = s"CheckboxXDisabled",
      .external = false,
      .w        = 10,
      .h        = 10
   },

   .scrdef = {
      .capS     = s"ListCapTop",
      .capE     = s"ListCapBottom",
      .scrl     = s"ListScrollbar",
      .notchgfx = s"ListScrollNotch",
      .notchhot = s"ListScrollNotchHot",
      .external = false,
      .scrlw    = 8,
      .scrlh    = 8
   },

   .slddef = {
      .gfx      = s"Slider",
      .snd      = s"player/cbi/slidertick",
      .notch    = s"SliderNotch",
      .notchhot = s"SliderNotchHot",
      .font     = s"cbifont",
      .external = false,
      .pad      = 2,
      .w        = 80,
      .h        = 7
   },
};

struct gui_presets const *Lith_GUIPreExtern(void)
{
   return &gui_p;
}

// EOF
