#include "lith_common.h"
#include "lith_player.h"

gui_button_preset_t const btndefault = {
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
};

gui_button_preset_t const btntab = {
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
};

gui_button_preset_t const btnexit = {
   .gfx      = "ExitButton.png",
   .hot      = "ExitButtonHot.png",
   .external = false,
   .w        = 11,
   .h        = 11
};

gui_button_preset_t const btnnext = {
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
};

gui_button_preset_t const btnprev = {
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
};

gui_button_preset_t const btnnexts = {
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
};

gui_button_preset_t const btnprevs = {
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
};

gui_button_preset_t const btnlist = {
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
};

gui_button_preset_t const btnlistsel = {
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
};

gui_button_preset_t const btnlistactive = {
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
};

gui_button_preset_t const btnlistactivated = {
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
};

gui_button_preset_t const btnbipmain = {
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
};

gui_button_preset_t const btnbipback = {
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
};

gui_button_preset_t const btndlgsel = {
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
};

gui_checkb_preset_t const cbxdefault = {
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
};

gui_checkb_preset_t const cbxsmall = {
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
};

gui_scroll_preset_t const scrdefault = {
   .capS     = "ListCapTop.png",
   .capE     = "ListCapBottom.png",
   .scrl     = "ListScrollbar.png",
   .notchgfx = "ListScrollNotch.png",
   .notchhot = "ListScrollNotchHot.png",
   .external = false,
   .scrlw    = 8,
   .scrlh    = 8
};

gui_slider_preset_t const slddefault = {
   .gfx      = "Slider.png",
   .snd      = "player/cbi/slidertick",
   .notch    = "SliderNotch.png",
   .notchhot = "SliderNotchHot.png",
   .font     = "cbifont",
   .external = false,
   .pad      = 2,
   .w        = 80,
   .h        = 7
};

// EOF

