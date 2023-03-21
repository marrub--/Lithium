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
#define PrintFill(c) \
   DrawCallV(sm_LW, (i32)(c))
#define PrintLine(x, y, z, w, c) \
   DrawCallV(sm_LL, (i32)(x), (i32)(y), (i32)(z), (i32)(w), (i32)(c))

#define BeginPrintFmt(...)   StrParamBegin(__VA_ARGS__)
#define BeginPrintStrN(s, n) (ACS_BeginPrint(), PrintStrN(s, n))
#define BeginPrintStr(s)     (ACS_BeginPrint(), PrintStr(s))
#define BeginPrintStrL(s)    (ACS_BeginPrint(), PrintStrL(s))
#define PrintText(...)       PrintText_str(ACS_EndStrParam(), __VA_ARGS__)

#define SetClip(x, y, w, h) \
   DrawCallV(sm_LC, (i32)(x), (i32)(y), (i32)(w), (i32)(h))
#define SetClipW(x, y, w, h, ww) \
   DrawCallV(sm_LC, (i32)(x), (i32)(y), (i32)(w), (i32)(h), (i32)(ww))
#define ClearClip() \
   DrawCallV(sm_LU)

#define SetSize(w, h) \
   DrawCallV(sm_LZ, (i32)(w), (i32)(h))

#define SetFade(n, time, speed) \
   DrawCallV(sm_LF, (i32)(n), (i32)(time), (i32)(speed), 1.0k)
#define SetFadeA(n, time, speed, alpha) \
   DrawCallV(sm_LF, (i32)(n), (i32)(time), (i32)(speed), (k32)(alpha))
#define CheckFade(n) \
   DrawCallI(sm_LX, (i32)(n))
#define GetFade(n) \
   DrawCallI(sm_LY, (i32)(n))

#define StartSound(...) \
   DrawCallV(sm_StartSound, __VA_ARGS__)
#define AmbientSound(snd, vol) \
   StartSound(snd, lch_ambient, CHANF_LOCAL|CHANF_UI, vol, ATTN_STATIC)

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

stkcall void DrawInit(void);
stkcall i32 Draw_GetCr(i32 n);
optargs(1) stkcall void TextSize(struct i32v2 *draw_text_size, str s, str font, i32 wrap);
stkcall struct i32v2 const *TextureSize(str tx);
stkcall optargs(3) void PrintText_str(str s, str font, i32 cr, i32 x, i32 xa, i32 y, i32 ya, i32 flg, k32 anum, i32 ww);
stkcall optargs(3) void PrintSprite(str name, i32 x, i32 xa, i32 y, i32 ya, i32 flg, k32 anum, i32 c);
stkcall optargs(3) void PrintSpriteClip(str name, i32 x, i32 xa, i32 y, i32 ya, i32 cx, i32 cy, i32 cw, i32 ch, i32 flg, k32 anum, i32 c);
stkcall optargs(1) void PrintRect(i32 x, i32 y, i32 w, i32 h, i32 c, i32 flg);
#endif

enum ZscName(FontNum) {
   font_misaki_gothic,
   font_misaki_mincho,
   font_jiskan16,
   font_num,
};

enum ZscName(Channel) {
   lch_auto,

   lch_weapon,
   lch_voice,
   lch_item,
   lch_body,
   lch_legacy_5,
   lch_legacy_6,
   lch_legacy_7,

   lch_altweapon,
   lch_body2,
   lch_body3,
   lch_shield,
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
   lch_doll,

   lch_step1,
   lch_step2,
   lch_weather1,
   lch_weather2,

   lch_ambient,
};

enum ZscName(DrawFlags) {
   _u_add        = 0x01,
   _u_fade       = 0x02,
   _u_alpha      = 0x04,
   _u_no_unicode = 0x08,
   _u_color      = 0x10,
   _u_clip       = 0x20,
};

#endif

/* EOF */
