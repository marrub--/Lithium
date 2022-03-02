// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Drawing functions.                                                       │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#if defined(GlobalCr)
#if !defined(GlobalCrH)
#define GlobalCrH GlobalCr
#endif

GlobalCr(blue) GlobalCr(green) GlobalCr(grey) GlobalCr(pink) GlobalCr(purple)
GlobalCr(red)  GlobalCr(yellow)

GlobalCr(rred) GlobalCr(rdor) GlobalCr(rora) GlobalCr(ryel) GlobalCr(rylg)
GlobalCr(rgrn) GlobalCr(rcya) GlobalCr(rblu) GlobalCr(rind) GlobalCr(rpur)
GlobalCr(rlav) GlobalCr(rpnk) GlobalCr(rhpn)

GlobalCrH(wsela1) GlobalCrH(wsela2) GlobalCrH(wsela3) GlobalCrH(wselas)
GlobalCrH(wselc1) GlobalCrH(wselc2) GlobalCrH(wselc3) GlobalCrH(wselcs)
GlobalCrH(wseld1) GlobalCrH(wseld2) GlobalCrH(wseld3) GlobalCrH(wselds)
GlobalCrH(wseli1) GlobalCrH(wseli2) GlobalCrH(wseli3) GlobalCrH(wselis)
GlobalCrH(wselm1) GlobalCrH(wselm2) GlobalCrH(wselm3) GlobalCrH(wselms)
#undef GlobalCr
#undef GlobalCrH
#else
#if !ZscOn
#define XArg(x, xa) (((i32)(x) & _xmask) | ((i32)(xa) << 28))
#define YArg(y, ya) (((i32)(y) & _ymask) | ((i32)(ya) << 29))

#define PrintRect(x, y, w, h, c) \
   DrawCallI(sm_LR, (i32)(x), (i32)(y), (i32)(w), (i32)(h), \
             (i32)(c))

#define PrintFill(c) \
   DrawCallI(sm_LW, (i32)(c))

#define PrintLine(x, y, z, w, c) \
   DrawCallI(sm_LL, (i32)(x), (i32)(y), (i32)(z), (i32)(w), (i32)(c))

#define PrintSprite(name, x, xa, y, ya) \
   DrawCallI(sm_LS, name, XArg(x, xa), YArg(y, ya), 0.0k, 0)

#define PrintSpriteA(name, x, xa, y, ya, alpha) \
   DrawCallI(sm_LS, name, XArg(x, xa), YArg(y, ya), (k32)(alpha), _u_alpha)

#define PrintSpriteF(name, x, xa, y, ya, num) \
   DrawCallI(sm_LS, name, XArg(x, xa), YArg(y, ya), (k32)(num), _u_fade)

#define PrintSpriteP(name, x, xa, y, ya) \
   DrawCallI(sm_LS, name, XArg(x, xa), YArg(y, ya), 0.0k, _u_add)

#define PrintSpriteAP(name, x, xa, y, ya, alpha) \
   DrawCallI(sm_LS, name, XArg(x, xa), YArg(y, ya), (k32)(alpha), _u_add | _u_alpha)

#define PrintSpriteFP(name, x, xa, y, ya, num) \
   DrawCallI(sm_LS, name, XArg(x, xa), YArg(y, ya), (k32)(num), _u_add | _u_fade)

#define PrintSpriteC(name, x, xa, y, ya, c) \
   DrawCallI(sm_LS, name, XArg(x, xa), YArg(y, ya), 0.0k, _u_color, (i32)(c))

#define PrintSpriteAC(name, x, xa, y, ya, alpha, c) \
   DrawCallI(sm_LS, name, XArg(x, xa), YArg(y, ya), (k32)(alpha), \
             _u_color|_u_alpha, (i32)(c))

#define PrintTextFmt(...)  StrParamBegin(__VA_ARGS__)
#define PrintTextChr(s, n) (ACS_BeginPrint(), PrintChars(s, n))
#define PrintTextChS(s)    (ACS_BeginPrint(), PrintChrSt(s))
#define PrintTextChL(s)    (ACS_BeginPrint(), PrintChrLi(s))
#define PrintTextEnd()     ACS_EndStrParam()

#define PrintText_str(s, font, cr, x, xa, y, ya) \
   DrawCallI(sm_LT, s, font, XArg(x, xa), YArg(y, ya), 0.0k, cr, 0)

#define PrintTextX_str(s, font, cr, x, xa, y, ya, flg) \
   DrawCallI(sm_LT, s, font, XArg(x, xa), YArg(y, ya), 0.0k, cr, flg)

#define PrintTextA_str(s, font, cr, x, xa, y, ya, alpha) \
   DrawCallI(sm_LT, s, font, XArg(x, xa), YArg(y, ya), (k32)(alpha), cr, _u_alpha)

#define PrintTextAX_str(s, font, cr, x, xa, y, ya, alpha, flg) \
   DrawCallI(sm_LT, s, font, XArg(x, xa), YArg(y, ya), (k32)(alpha), cr, _u_alpha|(i32)(flg))

#define PrintTextF_str(s, font, cr, x, xa, y, ya, num) \
   DrawCallI(sm_LT, s, font, XArg(x, xa), YArg(y, ya), (k32)(num), cr, _u_fade)

#define PrintTextFX_str(s, font, cr, x, xa, y, ya, num, flg) \
   DrawCallI(sm_LT, s, font, XArg(x, xa), YArg(y, ya), (k32)(num), cr, _u_fade|(i32)(flg))

#define PrintText(...)   PrintText_str  (PrintTextEnd(), __VA_ARGS__)
#define PrintTextX(...)  PrintTextX_str (PrintTextEnd(), __VA_ARGS__)
#define PrintTextA(...)  PrintTextA_str (PrintTextEnd(), __VA_ARGS__)
#define PrintTextAX(...) PrintTextAX_str(PrintTextEnd(), __VA_ARGS__)
#define PrintTextF(...)  PrintTextF_str (PrintTextEnd(), __VA_ARGS__)
#define PrintTextFX(...) PrintTextFX_str(PrintTextEnd(), __VA_ARGS__)

#define SetClip(x, y, w, h) \
   DrawCallI(sm_LC, (i32)(x), (i32)(y), (i32)(w), (i32)(h))
#define SetClipW(x, y, w, h, ww) \
   DrawCallI(sm_LC, (i32)(x), (i32)(y), (i32)(w), (i32)(h), (i32)(ww))
#define ClearClip() \
   DrawCallI(sm_LC)

#define SetSize(w, h) \
   DrawCallI(sm_LZ, (i32)(w), (i32)(h))

#define SetFade(n, time, speed) \
   DrawCallI(sm_LF, (i32)(n), (i32)(time), (i32)(speed), 1.0k)

#define SetFadeA(n, time, speed, alpha) \
   DrawCallI(sm_LF, (i32)(n), (i32)(time), (i32)(speed), (k32)(alpha))

#define CheckFade(n) \
   DrawCallI(sm_LX, (i32)(n))

#define GetFade(n) \
   DrawCallI(sm_LY, (i32)(n))

#define StartSound(...) \
   DrawCallI(sm_StartSound, __VA_ARGS__)

#define AmbientSound(snd, vol) \
   StartSound(snd, lch_auto, 0, vol, ATTN_STATIC)

#define EndDrawing() \
   DrawCallI(sm_LE)

enum {
   CHANF_LISTENERZ   = 8,
   CHANF_MAYBE_LOCAL = 16,
   CHANF_UI          = 32,
   CHANF_NOPAUSE     = 64,
   CHANF_LOOP        = 256,
   CHANF_NOSTOP      = 4096,
   CHANF_OVERLAP     = 8192,
   CHANF_LOCAL       = 16384,
};

#define Cr(name) globalcolors.name

struct globalcolors {
   #define GlobalCr(name) i32 name;
   #include "m_drawing.h"
};

extern struct globalcolors globalcolors;

enum {
   _gcr_first = 'a',
   _gcr_last = 0x7a,
   _gcr_class,
   #define GlobalCr(name) _gcr_##name,
   #define GlobalCrH(name)
   #include "m_drawing.h"
   _gcr_max,
};

enum {
   _ssld_fixed,
   _ssld_slide,
   _ssld_under,
   _ssld_max,
};

enum {
   _sfont_small,
   _sfont_mid,
   _sfont_italic,
   _sfont_max,
};

stkcall void Draw_Init(void);
stkcall i32 Draw_GetCr(i32 n);
#endif

enum ZscName(FontNum) {
   font_misaki_gothic,
   font_misaki_mincho,
   font_k6x8,
   font_jiskan16,
   font_num,
};

enum ZscName(Channel) {
   lch_auto,

   lch_weapon,
   lch_voice,
   lch_item,
   lch_body,

   lch_altweapon,
   lch_body2,
   lch_body3,
   lch_dryweapon,
   lch_idleweapon,
   lch_item2,
   lch_reload,
   lch_reload2,
   lch_reload3,
   lch_scope,
   lch_voice2,
   lch_weapon2,
   lch_weapon3,

   lch_step,
   lch_weather1,
   lch_weather2,
   lch_depth,
};

enum ZscName(DrawFlags) {
   _u_add        = 0x01,
   _u_fade       = 0x02,
   _u_alpha      = 0x04,
   _u_no_unicode = 0x08,
   _u_color      = 0x10,
};

enum ZscName(XAlign) {
   _xc    = 0x00000000,
   _xl    = 0x10000000,
   _xr    = 0x20000000,
   _x3    = 0x30000000,
   _x4    = 0x40000000,
   _x5    = 0x50000000,
   _x6    = 0x60000000,
   _xmask = 0x8FFFFFFF,
   _xflag = 0x70000000,
};

enum ZscName(YAlign) {
   _yc    = 0x00000000,
   _yt    = 0x20000000,
   _yb    = 0x40000000,
   _ymask = 0x9FFFFFFF,
   _yflag = 0x60000000,
};

#endif

/* EOF */
