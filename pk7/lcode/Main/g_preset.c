#include "lith_common.h"
#include "lith_player.h"

#define BtnDefault \
   .snd  = "player/cbi/buttonpress", \
   .cdef = "j", \
   .cact = "g", \
   .chot = "k", \
   .font = "CBIFONT"

#define CbxDefault \
   .sndup  = "player/cbi/clickon", \
   .snddn  = "player/cbi/clickoff", \
   .w = 10, \
   .h = 10, \
   .font = "CNFONT", \
   .chkgfx = "CheckboxX.png", \
   .chkhot = "CheckboxXHot.png", \
   .chkact = "CheckboxXActive.png", \
   .chkdis = "CheckboxXDisabled.png"


//----------------------------------------------------------------------------
// Extern Objects
//

gui_button_preset_t const btndefault = {
   BtnDefault,
   .gfx  = "Button.png",
   .hot  = "ButtonHot.png",
   .cdis = "m",
   .w = 48,
   .h = 16
};

gui_button_preset_t const btntab = {
   BtnDefault,
   .gfx  = "Tab.png",
   .hot  = "TabHot.png",
   .cdis = "i",
   .w = 46,
   .h = 11
};

gui_button_preset_t const btnexit = {
   .gfx  = "ExitButton.png",
   .hot  = "ExitButtonHot.png",
   .w = 11,
   .h = 11
};

gui_button_preset_t const btnnext = {
   BtnDefault,
   .gfx  = "BtnNext.png",
   .hot  = "BtnNextHot.png",
   .w = 8,
   .h = 11
};

gui_button_preset_t const btnprev = {
   BtnDefault,
   .gfx  = "BtnPrev.png",
   .hot  = "BtnPrevHot.png",
   .w = 8,
   .h = 11
};

gui_button_preset_t const btnnexts = {
   BtnDefault,
   .gfx  = "BtnNextSmall.png",
   .hot  = "BtnNextSmallHot.png",
   .w = 6,
   .h = 9
};

gui_button_preset_t const btnprevs = {
   BtnDefault,
   .gfx  = "BtnPrevSmall.png",
   .hot  = "BtnPrevSmallHot.png",
   .w = 6,
   .h = 9
};

gui_button_preset_t const btnlist = {
   BtnDefault,
   .gfx  = "ListButton.png",
   .hot  = "ListButtonHot.png",
   .cdis = "u",
   .w = 80,
   .h = 9
};

gui_button_preset_t const btnlistsel = {
   BtnDefault,
   .gfx  = "ListButton.png",
   .hot  = "ListButtonHot.png",
   .cdis = "i",
   .w = 80,
   .h = 9
};

gui_button_preset_t const btnlistactive = {
   BtnDefault,
   .gfx  = "ListButtonActive.png",
   .hot  = "ListButtonActiveHot.png",
   .cdis = "i",
   .w = 80,
   .h = 9
};

gui_button_preset_t const btnlistactivated = {
   BtnDefault,
   .gfx  = "ListButtonActivated.png",
   .hot  = "ListButtonActiveHot.png",
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

gui_button_preset_t const btndlgsel = {
   BtnDefault,
   .chot = "d",
   .gfx  = "lgfx/Dialogue/SelectBack.png",
   .hot  = "lgfx/Dialogue/SelectBackHot.png",
   .cdis = "i",
   .w = 240,
   .h = 14,
   .external = true
};

gui_checkb_preset_t const cbxdefault = {
   CbxDefault,
   .gfx    = "Checkbox.png",
   .hot    = "CheckboxHot.png",
   .dis    = "CheckboxDisabled.png"
};

gui_checkb_preset_t const cbxsmall = {
   CbxDefault,
   .gfx    = "CheckboxSmall.png",
   .hot    = "CheckboxSmallHot.png",
   .dis    = "CheckboxSmallDisabled.png"
};

gui_scroll_preset_t const scrdefault = {
   .capS  = "ListCapTop.png",
   .capE  = "ListCapBottom.png",
   .scrl  = "ListScrollbar.png",
   .scrlw = 8,
   .scrlh = 8,
   .notchgfx = "ListScrollNotch.png",
   .notchhot = "ListScrollNotchHot.png"
};

gui_slider_preset_t const slddefault = {
   .gfx    = "Slider.png",
   .snd    = "player/cbi/slidertick",
   .notch  = "SliderNotch.png",
   .notchhot = "SliderNotchHot.png",
   .pad = 2,
   .w   = 80,
   .h = 7
};

// EOF

