/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * GUI presets.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"

struct gui_presets const gui_p = {
   .btndef = {
      .gfx      = "Button",
      .hot      = "ButtonHot",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "m",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/buttonpress",
      .external = false,
      .w        = 48,
      .h        = 16,
      .ax = 4, .ay = 0
   },

   .btntab = {
      .gfx      = "Tab",
      .hot      = "TabHot",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/tabswitch",
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
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/right",
      .external = false,
      .w        = 8,
      .h        = 11,
      .ax = 4, .ay = 0
   },

   .btnprev = {
      .gfx      = "BtnPrev",
      .hot      = "BtnPrevHot",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/left",
      .external = false,
      .w        = 8,
      .h        = 11,
      .ax = 4, .ay = 0
   },

   .btnnexts = {
      .gfx      = "BtnNextSmall",
      .hot      = "BtnNextSmallHot",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/buttoncurs",
      .external = false,
      .w        = 6,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnprevs = {
      .gfx      = "BtnPrevSmall",
      .hot      = "BtnPrevSmallHot",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/buttoncurs",
      .external = false,
      .w        = 6,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnlist = {
      .gfx      = "ListButton",
      .hot      = "ListButtonHot",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "u",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/clicklst",
      .external = false,
      .w        = 80,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnlistsel = {
      .gfx      = "ListButton",
      .hot      = "ListButtonHot",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/clicklst",
      .external = false,
      .w        = 80,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnlistactive = {
      .gfx      = "ListButtonActive",
      .hot      = "ListButtonActiveHot",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/clicklst",
      .external = false,
      .w        = 80,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnlistactivated = {
      .gfx      = "ListButtonActivated",
      .hot      = "ListButtonActiveHot",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "i",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/clicklst",
      .external = false,
      .w        = 80,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btnbipmain = {
      .gfx      = nil,
      .hot      = nil,
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "-",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/buttonpress",
      .external = false,
      .w        = 180,
      .h        = 9,
      .ax = 1, .ay = 0
   },

   .btnbipback = {
      .gfx      = nil,
      .hot      = nil,
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "n",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/buttonpress",
      .external = false,
      .w        = 48,
      .h        = 9,
      .ax = 4, .ay = 0
   },

   .btndlgsel = {
      .gfx      = ":Dialogue:SelectBack",
      .hot      = ":Dialogue:SelectBackHot",
      .cdef     = "j",
      .cact     = "g",
      .chot     = "d",
      .cdis     = "i",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/dlgclick",
      .external = true,
      .w        = 240,
      .h        = 14,
      .ax = 4, .ay = 0
   },

   .btnclear = {
      .gfx      = nil,
      .hot      = nil,
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "n",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/buttonpress",
      .external = false,
      .w        = 20,
      .h        = 8,
      .ax = 1, .ay = 1
   },

   .btnnote = {
      .gfx      = nil,
      .hot      = nil,
      .cdef     = "j",
      .cact     = "g",
      .chot     = "k",
      .cdis     = "-",
      .font     = s"smallfnt",
      .snd      = s"player/cbi/buttoncurs",
      .external = false,
      .w        = 200,
      .h        = 30,
      .ax = 1, .ay = 1
   },

   .cbxdef = {
      .gfx      = "Checkbox",
      .hot      = "CheckboxHot",
      .dis      = "CheckboxDisabled",
      .chkgfx   = "CheckboxX",
      .chkhot   = "CheckboxXHot",
      .chkact   = "CheckboxXActive",
      .chkdis   = "CheckboxXDisabled",
      .sndup    = s"player/cbi/clickon",
      .snddn    = s"player/cbi/clickoff",
      .external = false,
      .w        = 10,
      .h        = 10
   },

   .cbxsmall = {
      .gfx      = "CheckboxSmall",
      .hot      = "CheckboxSmallHot",
      .dis      = "CheckboxSmallDisabled",
      .chkgfx   = "CheckboxX",
      .chkhot   = "CheckboxXHot",
      .chkact   = "CheckboxXActive",
      .chkdis   = "CheckboxXDisabled",
      .sndup    = s"player/cbi/clickon",
      .snddn    = s"player/cbi/clickoff",
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
      .notch    = "SliderNotch",
      .notchhot = "SliderNotchHot",
      .snd      = s"player/cbi/slidertick",
      .font     = s"smallfnt",
      .external = false,
      .pad      = 2,
      .w        = 80,
      .h        = 7
   },
};

/* EOF */
