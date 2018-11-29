// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"

struct gui_presets const guipre = {
   .btndef = {
      .gfx      = c"Button",
      .hot      = c"ButtonHot",
      .snd      = c"player/cbi/buttonpress",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"k",
      .cdis     = c"m",
      .font     = c"cbifont",
      .external = false,
      .w        = 48,
      .h        = 16,
      .ax = 4, .ay = 0
   },

   .btntab = {
      .gfx      = c"Tab",
      .hot      = c"TabHot",
      .snd      = c"player/cbi/tabswitch",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"k",
      .cdis     = c"i",
      .font     = c"cbifont",
      .external = false,
      .w        = 46,
      .h        = 11,
      .ax = 4, .ay = 0
   },

   .btnexit = {
      .gfx      = c"ExitButton",
      .hot      = c"ExitButtonHot",
      .external = false,
      .w        = 11,
      .h        = 11,
      .ax = 4, .ay = 0
   },

   .btnnext = {
      .gfx      = c"BtnNext",
      .hot      = c"BtnNextHot",
      .snd      = c"player/cbi/right",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"k",
      .cdis     = c"i",
      .font     = c"cbifont",
      .external = false,
      .w        = 8,
      .h        = 11,
      .ax = 4, .ay = 0
   },

   .btnprev = {
      .gfx      = c"BtnPrev",
      .hot      = c"BtnPrevHot",
      .snd      = c"player/cbi/left",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"k",
      .cdis     = c"i",
      .font     = c"cbifont",
      .external = false,
      .w        = 8,
      .h        = 11,
      .ax = 4, .ay = 0
   },

   .btnnexts = {
      .gfx      = c"BtnNextSmall",
      .hot      = c"BtnNextSmallHot",
      .snd      = c"player/cbi/buttoncurs",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"k",
      .cdis     = c"i",
      .font     = c"cbifont",
      .external = false,
      .w        = 6,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnprevs = {
      .gfx      = c"BtnPrevSmall",
      .hot      = c"BtnPrevSmallHot",
      .snd      = c"player/cbi/buttoncurs",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"k",
      .cdis     = c"i",
      .font     = c"cbifont",
      .external = false,
      .w        = 6,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnlist = {
      .gfx      = c"ListButton",
      .hot      = c"ListButtonHot",
      .snd      = c"player/cbi/clicklst",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"k",
      .cdis     = c"u",
      .font     = c"cbifont",
      .external = false,
      .w        = 80,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnlistsel = {
      .gfx      = c"ListButton",
      .hot      = c"ListButtonHot",
      .snd      = c"player/cbi/clicklst",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"k",
      .cdis     = c"i",
      .font     = c"cbifont",
      .external = false,
      .w        = 80,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnlistactive = {
      .gfx      = c"ListButtonActive",
      .hot      = c"ListButtonActiveHot",
      .snd      = c"player/cbi/clicklst",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"k",
      .cdis     = c"i",
      .font     = c"cbifont",
      .external = false,
      .w        = 80,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnlistactivated = {
      .gfx      = c"ListButtonActivated",
      .hot      = c"ListButtonActiveHot",
      .snd      = c"player/cbi/clicklst",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"k",
      .cdis     = c"i",
      .font     = c"cbifont",
      .external = false,
      .w        = 80,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnbipmain = {
      .gfx      = null,
      .hot      = null,
      .snd      = c"player/cbi/buttonpress",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"k",
      .cdis     = c"-",
      .font     = c"cbifont",
      .external = false,
      .w        = 180,
      .h        = 9,
      .ax = 1, .ay = 0
   },

   .btnbipback = {
      .gfx      = null,
      .hot      = null,
      .snd      = c"player/cbi/buttonpress",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"k",
      .cdis     = c"n",
      .font     = c"cbifont",
      .external = false,
      .w        = 48,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btndlgsel = {
      .gfx      = c":Dialogue:SelectBack",
      .hot      = c":Dialogue:SelectBackHot",
      .snd      = c"player/cbi/dlgclick",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"d",
      .cdis     = c"i",
      .font     = c"cbifont",
      .external = true,
      .w        = 240,
      .h        = 14,
      .ax = 4, .ay = 0
   },

   .btnclear = {
      .gfx      = null,
      .hot      = null,
      .snd      = c"player/cbi/buttonpress",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"k",
      .cdis     = c"n",
      .font     = c"cbifont",
      .external = false,
      .w        = 20,
      .h        = 8,
      .ax = 1, .ay = 1
   },

   .btnnote = {
      .gfx      = null,
      .hot      = null,
      .snd      = c"player/cbi/buttoncurs",
      .cdef     = c"j",
      .cact     = c"g",
      .chot     = c"k",
      .cdis     = c"-",
      .font     = c"cbifont",
      .external = false,
      .w        = 200,
      .h        = 30,
      .ax = 1, .ay = 1
   },

   .cbxdef = {
      .gfx      = "Checkbox",
      .hot      = "CheckboxHot",
      .dis      = "CheckboxDisabled",
      .sndup    = "player/cbi/clickon",
      .snddn    = "player/cbi/clickoff",
      .chkgfx   = "CheckboxX",
      .chkhot   = "CheckboxXHot",
      .chkact   = "CheckboxXActive",
      .chkdis   = "CheckboxXDisabled",
      .external = false,
      .w        = 10,
      .h        = 10
   },

   .cbxsmall = {
      .gfx      = "CheckboxSmall",
      .hot      = "CheckboxSmallHot",
      .dis      = "CheckboxSmallDisabled",
      .sndup    = "player/cbi/clickon",
      .snddn    = "player/cbi/clickoff",
      .chkgfx   = "CheckboxX",
      .chkhot   = "CheckboxXHot",
      .chkact   = "CheckboxXActive",
      .chkdis   = "CheckboxXDisabled",
      .external = false,
      .w        = 10,
      .h        = 10
   },

   .scrdef = {
      .capS     = "ListCapTop",
      .capE     = "ListCapBottom",
      .scrl     = "ListScrollbar",
      .notchgfx = "ListScrollNotch",
      .notchhot = "ListScrollNotchHot",
      .external = false,
      .scrlw    = 8,
      .scrlh    = 8
   },

   .slddef = {
      .gfx      = "Slider",
      .snd      = "player/cbi/slidertick",
      .notch    = "SliderNotch",
      .notchhot = "SliderNotchHot",
      .font     = "cbifont",
      .external = false,
      .pad      = 2,
      .w        = 80,
      .h        = 7
   },
};

struct gui_presets const *Lith_GUIPreExtern(void)
{
   return &guipre;
}

// EOF

