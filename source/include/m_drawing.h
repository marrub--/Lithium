/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Drawing functions.
 *
 * ---------------------------------------------------------------------------|
 */
/* decompat-out pk7/lzscript/Constants/m_drawing.zsc */

/* decompat-cut */
#define BitArg(x, mask, shift) (((u32)(x) & mask) << shift)
#define SignArg(x, mask) (((u32)(x) & mask) | ((u32)(x) & 0x80000000))

#define XBitArg(x, xa) (SignArg(x, 0x0FFFFFFF) | BitArg(xa, 7, 28))
#define YBitArg(y, ya) (SignArg(y, 0x1FFFFFFF) | BitArg(ya, 3, 29))

#define PrintRectArgs0(x, y) (BitArg(x, 0xffff, 0) | BitArg(y, 0xffff, 16))
#define PrintRectArgs1(w, h) PrintRectArgs0(w, h)

#define PrintRect(x, y, w, h, c) \
   DrawCallI(sm_LR, PrintRectArgs0(x, y), PrintRectArgs1(w, h), c)

#define PrintSpriteArgs0(x,      xa) XBitArg(x, xa)
#define PrintSpriteArgs1(y,      ya) YBitArg(y, ya)
#define PrintSpriteArgs2(alpha, flg) (BitArg(alpha, 0xff, 0) | (u32)(flg))

#define PrintSpriteArgs_N(x, xa, y, ya, flg) \
   PrintSpriteArgs0(x, xa), \
   PrintSpriteArgs1(y, ya), \
   PrintSpriteArgs2(0, flg)

#define PrintSpriteArgs_A(x, xa, y, ya, alpha, flg) \
   PrintSpriteArgs0(x, xa), \
   PrintSpriteArgs1(y, ya), \
   PrintSpriteArgs2(k32_to_byte(alpha), 0x400 | (flg))

#define PrintSpriteArgs_F(x, xa, y, ya, num, flg) \
   PrintSpriteArgs0(x, xa), \
   PrintSpriteArgs1(y, ya), \
   PrintSpriteArgs2(num, 0x200 | (flg))

#define PrintSprite(name, x, xa, y, ya) \
   DrawCallI(sm_LS, name, PrintSpriteArgs_N(x, xa, y, ya, 0))

#define PrintSpriteA(name, x, xa, y, ya, alpha) \
   DrawCallI(sm_LS, name, PrintSpriteArgs_A(x, xa, y, ya, alpha, 0))

#define PrintSpriteF(name, x, xa, y, ya, num) \
   DrawCallI(sm_LS, name, PrintSpriteArgs_F(x, xa, y, ya, num, 0))

#define PrintSpriteP(name, x, xa, y, ya) \
   DrawCallI(sm_LS, name, PrintSpriteArgs_N(x, xa, y, ya, 0x100))

#define PrintSpriteAP(name, x, xa, y, ya, alpha) \
   DrawCallI(sm_LS, name, PrintSpriteArgs_A(x, xa, y, ya, alpha, 0x100))

#define PrintSpriteFP(name, x, xa, y, ya, num) \
   DrawCallI(sm_LS, name, PrintSpriteArgs_F(x, xa, y, ya, num, 0x100))

#define PrintTextFmt(...)  StrParamBegin(__VA_ARGS__)
#define PrintTextStr(s)    (ACS_BeginPrint(), ACS_PrintString(s))
#define PrintTextChr(s, n) (ACS_BeginPrint(), PrintChars(s, n))
#define PrintTextChS(s)    (ACS_BeginPrint(), PrintChrSt(s))

#define PrintTextArgs0(x, xa)          XBitArg(x, xa)
#define PrintTextArgs1(y, ya)          YBitArg(y, ya)
#define PrintTextArgs2(cr, alpha, flg) (BitArg(cr,    0xff, 0) | \
                                        BitArg(alpha, 0xff, 8) | \
                                        (u32)(flg))

#define PrintTextArgs_N(cr, flg, x, xa, y, ya) \
   PrintTextArgs0(x, xa), \
   PrintTextArgs1(y, ya), \
   PrintTextArgs2(cr, 0, flg)

#define PrintTextArgs_A(cr, flg, x, xa, y, ya, alpha) \
   PrintTextArgs0(x, xa), \
   PrintTextArgs1(y, ya), \
   PrintTextArgs2(cr, k32_to_byte(alpha), (flg) | 0x20000)

#define PrintTextArgs_F(cr, flg, x, xa, y, ya, num) \
   PrintTextArgs0(x, xa), \
   PrintTextArgs1(y, ya), \
   PrintTextArgs2(cr, num, (flg) | 0x10000)

#define PrintTextEnd() ACS_EndStrParam()

#define PrintText_str(s, font, cr, x, xa, y, ya) \
   DrawCallI(sm_LT, s, font, PrintTextArgs_N(cr, 0, x, xa, y, ya))

#define PrintTextX_str(s, font, cr, x, xa, y, ya, flg) \
   DrawCallI(sm_LT, s, font, PrintTextArgs_N(cr, flg, x, xa, y, ya))

#define PrintTextA_str(s, font, cr, x, xa, y, ya, alpha) \
   DrawCallI(sm_LT, s, font, PrintTextArgs_A(cr, 0, x, xa, y, ya, alpha))

#define PrintTextAX_str(s, font, cr, x, xa, y, ya, alpha, flg) \
   DrawCallI(sm_LT, s, font, PrintTextArgs_A(cr, flg, x, xa, y, ya, alpha))

#define PrintTextF_str(s, font, cr, x, xa, y, ya, num) \
   DrawCallI(sm_LT, s, font, PrintTextArgs_F(cr, 0, x, xa, y, ya, num))

#define PrintTextFX_str(s, font, cr, x, xa, y, ya, num, flg) \
   DrawCallI(sm_LT, s, font, PrintTextArgs_F(cr, flg, x, xa, y, ya, num))

#define PrintText(...)   PrintText_str  (PrintTextEnd(), __VA_ARGS__)
#define PrintTextX(...)  PrintTextX_str (PrintTextEnd(), __VA_ARGS__)
#define PrintTextA(...)  PrintTextA_str (PrintTextEnd(), __VA_ARGS__)
#define PrintTextAX(...) PrintTextAX_str(PrintTextEnd(), __VA_ARGS__)
#define PrintTextF(...)  PrintTextF_str (PrintTextEnd(), __VA_ARGS__)
#define PrintTextFX(...) PrintTextFX_str(PrintTextEnd(), __VA_ARGS__)

#define SetClipArgs0(x, y) (BitArg(x, 0xffff, 0) | BitArg(y, 0xffff, 16))
#define SetClipArgs1(w, h) SetClipArgs0(w, h)

#define SetClip(x, y, w, h) \
   DrawCallI(sm_LC, SetClipArgs0(x, y), SetClipArgs1(w, h))

#define SetClipW(x, y, w, h, ww) \
   DrawCallI(sm_LC, SetClipArgs0(x, y), SetClipArgs1(w, h), (u32)(ww))

#define ClearClip() \
   DrawCallI(sm_LC)

#define SetSize(w, h) \
   DrawCallI(sm_LZ, (i32)(w), (i32)(h))

#define SetFadeArgs(num, time, speed, alpha) \
   (BitArg(num, 0xff, 0)  | \
    BitArg(time, 0xff, 8)  | \
    BitArg(speed, 0xff, 16) | \
    BitArg(k32_to_byte(alpha), 0xff, 24))

#define SetFade(n, time, speed) \
   DrawCallI(sm_LF, SetFadeArgs(n, time, speed, 1.0))

#define SetFadeA(n, time, speed, alpha) \
   DrawCallI(sm_LF, SetFadeArgs(n, time, speed, alpha))

#define CheckFade(n) \
   DrawCallI(sm_LX, n)

#define StartSound(...) \
   DrawCallI(sm_StartSound, __VA_ARGS__)

#define CrBlue   "\C[Lith_Blue]"
#define CrGreen  "\C[Lith_Green]"
#define CrRed    "\C[Lith_Red]"
#define CrPurple "\C[Lith_Purple]"
#define CrGrey   "\C[Lith_Grey]"

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

enum {
   ptf_no_utf = 0x40000,
   ptf_add    = 0x80000,
};
/* decompat-end */

enum /* Lith_FontNum */
{
   font_misaki_gothic,
   font_misaki_mincho,
   font_k6x8,
   font_jiskan16,
   font_num,
};

enum /* Lith_Channel */
{
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
};

/* EOF */
