// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"

struct gui_presets const guipre = {
   .btndef = {
      .gfx      = "Button.png",
      .hot      = "ButtonHot.png",
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "m",
      .font     = "cbifont",
      .external = false,
      .w        = 48,
      .h        = 16
   },

   .btntab = {
      .gfx      = "Tab.png",
      .hot      = "TabHot.png",
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 46,
      .h        = 11
   },

   .btnexit = {
      .gfx      = "ExitButton.png",
      .hot      = "ExitButtonHot.png",
      .external = false,
      .w        = 11,
      .h        = 11
   },

   .btnnext = {
      .gfx      = "BtnNext.png",
      .hot      = "BtnNextHot.png",
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 8,
      .h        = 11
   },

   .btnprev = {
      .gfx      = "BtnPrev.png",
      .hot      = "BtnPrevHot.png",
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 8,
      .h        = 11
   },

   .btnnexts = {
      .gfx      = "BtnNextSmall.png",
      .hot      = "BtnNextSmallHot.png",
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 6,
      .h        = 9
   },

   .btnprevs = {
      .gfx      = "BtnPrevSmall.png",
      .hot      = "BtnPrevSmallHot.png",
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 6,
      .h        = 9
   },

   .btnlist = {
      .gfx      = "ListButton.png",
      .hot      = "ListButtonHot.png",
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "u",
      .font     = "cbifont",
      .external = false,
      .w        = 80,
      .h        = 9
   },

   .btnlistsel = {
      .gfx      = "ListButton.png",
      .hot      = "ListButtonHot.png",
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 80,
      .h        = 9
   },

   .btnlistactive = {
      .gfx      = "ListButtonActive.png",
      .hot      = "ListButtonActiveHot.png",
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 80,
      .h        = 9
   },

   .btnlistactivated = {
      .gfx      = "ListButtonActivated.png",
      .hot      = "ListButtonActiveHot.png",
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = "cbifont",
      .external = false,
      .w        = 80,
      .h        = 9
   },

   .btnbipmain = {
      .gfx      = null,
      .hot      = null,
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "n",
      .font     = "cbifont",
      .external = false,
      .w        = 180,
      .h        = 9
   },

   .btnbipback = {
      .gfx      = null,
      .hot      = null,
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "n",
      .font     = "cbifont",
      .external = false,
      .w        = 48,
      .h        = 9
   },

   .btndlgsel = {
      .gfx      = "lgfx/Dialogue/SelectBack.png",
      .hot      = "lgfx/Dialogue/SelectBackHot.png",
      .snd      = "player/cbi/buttonpress",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "d",
      .cdis     = "i",
      .font     = "cbifont",
      .external = true,
      .w        = 240,
      .h        = 14
   },

   .cbxdef = {
      .gfx      = "Checkbox.png",
      .hot      = "CheckboxHot.png",
      .dis      = "CheckboxDisabled.png",
      .sndup    = "player/cbi/clickon",
      .snddn    = "player/cbi/clickoff",
      .chkgfx   = "CheckboxX.png",
      .chkhot   = "CheckboxXHot.png",
      .chkact   = "CheckboxXActive.png",
      .chkdis   = "CheckboxXDisabled.png",
      .external = false,
      .w        = 10,
      .h        = 10
   },

   .cbxsmall = {
      .gfx      = "CheckboxSmall.png",
      .hot      = "CheckboxSmallHot.png",
      .dis      = "CheckboxSmallDisabled.png",
      .sndup    = "player/cbi/clickon",
      .snddn    = "player/cbi/clickoff",
      .chkgfx   = "CheckboxX.png",
      .chkhot   = "CheckboxXHot.png",
      .chkact   = "CheckboxXActive.png",
      .chkdis   = "CheckboxXDisabled.png",
      .external = false,
      .w        = 10,
      .h        = 10
   },

   .scrdef = {
      .capS     = "ListCapTop.png",
      .capE     = "ListCapBottom.png",
      .scrl     = "ListScrollbar.png",
      .notchgfx = "ListScrollNotch.png",
      .notchhot = "ListScrollNotchHot.png",
      .external = false,
      .scrlw    = 8,
      .scrlh    = 8
   },

   .slddef = {
      .gfx      = "Slider.png",
      .snd      = "player/cbi/slidertick",
      .notch    = "SliderNotch.png",
      .notchhot = "SliderNotchHot.png",
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

