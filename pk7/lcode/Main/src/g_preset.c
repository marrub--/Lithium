#include "lith_common.h"
#include "lith_player.h"

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

gui_button_preset_t const btnnexts = {
   BtnDefault,
   .gfx  = "lgfx/UI/BtnNextSmall.png",
   .hot  = "lgfx/UI/BtnNextSmallHot.png",
   .w = 6,
   .h = 9
};

gui_button_preset_t const btnprevs = {
   BtnDefault,
   .gfx  = "lgfx/UI/BtnPrevSmall.png",
   .hot  = "lgfx/UI/BtnPrevSmallHot.png",
   .w = 6,
   .h = 9
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
   .dis    = "lgfx/UI/CheckboxDisabled.png",
   .sndup  = "player/cbi/clickon",
   .snddn  = "player/cbi/clickoff",
   .chkgfx = "lgfx/UI/CheckboxX.png",
   .chkhot = "lgfx/UI/CheckboxXHot.png",
   .chkact = "lgfx/UI/CheckboxXActive.png",
   .chkdis = "lgfx/UI/CheckboxXDisabled.png",
   .font = "CNFONT",
   .w = 10,
   .h = 10
};

gui_checkb_preset_t const cbxsmall = {
   .gfx    = "lgfx/UI/CheckboxSmall.png",
   .hot    = "lgfx/UI/CheckboxSmallHot.png",
   .dis    = "lgfx/UI/CheckboxSmallDisabled.png",
   .sndup  = "player/cbi/clickon",
   .snddn  = "player/cbi/clickoff",
   .chkgfx = "lgfx/UI/CheckboxX.png",
   .chkhot = "lgfx/UI/CheckboxXHot.png",
   .chkact = "lgfx/UI/CheckboxXActive.png",
   .chkdis = "lgfx/UI/CheckboxXDisabled.png",
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
   .notchh   = 3,
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

// EOF

