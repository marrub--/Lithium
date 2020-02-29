/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Drawing functions.
 *
 * ---------------------------------------------------------------------------|
 */

/* Not 1.0 / 35.0 or even 0.028 because ZDoom is stupid. */
#define TS 0.029

#define PrintRectArgs0(x, y) ((u32)(x) | ((u32)(y) << 16))
#define PrintRectArgs1(w, h) PrintRectArgs0(w, h)

#define PrintRect(x, y, w, h, c) \
   DrawCallI(sm_LR, PrintRectArgs0(x, y), PrintRectArgs1(w, h), c)

#define PrintSprite(name, x, xa, y, ya) \
   DrawCallI(sm_LS, name, (i32)(x), (i32)(y), (i32)(xa), (i32)(ya))

#define PrintSpriteA(name, x, xa, y, ya, a) \
   DrawCallI(sm_LS, name, (i32)(x), (i32)(y), (i32)(xa), (i32)(ya), (k32)(a))

#define PrintSpriteF(name, x, xa, y, ya, n) \
   DrawCallI(sm_LS, name, (i32)(x), (i32)(y), (i32)(xa), (i32)(ya), (k32)(-(n) - 2))

#define PrintTextFmt(...) StrParamBegin(__VA_ARGS__)
#define PrintTextStr(s)    (ACS_BeginPrint(), ACS_PrintString(s))
#define PrintTextChr(s, n) (ACS_BeginPrint(), PrintChars(s, n))
#define PrintTextChS(s)    (ACS_BeginPrint(), PrintChrSt(s))

#define PrintTextArg0(cr, flg) \
   (((u32)(cr) & 0xff) | (u32)(flg))

#define PrintTextArg1(x, xa) \
   ( \
      ((u32)(x) & 0x0FFFFFFF) | \
      ((u32)(x) & 0x80000000) | \
      ((u32)(xa) << 28) \
   )

#define PrintTextArg2(y, ya) \
   ( \
      ((u32)(y) & 0x1FFFFFFF) | \
      ((u32)(y) & 0x80000000) | \
      ((u32)(ya) << 29) \
   )

#define PrintTextArgs_N(cr, flg, x, y, xa, ya) \
   PrintTextArg0(cr, flg), \
   PrintTextArg1(x, xa), \
   PrintTextArg2(y, ya)

#define PrintTextArgs_A(cr, flg, x, y, xa, ya, a) \
   PrintTextArg0(cr, (flg) | 0x40000000 | ((u32)((k32)(a) * 0x3f) << 24)), \
   PrintTextArg1(x, xa), \
   PrintTextArg2(y, ya)

#define PrintTextArgs_F(cr, flg, x, y, xa, ya, n) \
   PrintTextArg0(cr, (flg) | (((u32)(n) + 1) << 24)), \
   PrintTextArg1(x, xa), \
   PrintTextArg2(y, ya)

#define PrintTextEnd() ACS_EndStrParam()

#define PrintText_str(s, font, cr, x, xa, y, ya) \
   DrawCallI(sm_LT, s, font, PrintTextArgs_N(cr, 0, x, y, xa, ya))

#define PrintTextX_str(s, font, cr, x, xa, y, ya) \
   DrawCallI(sm_LT, s, font, PrintTextArgs_N(cr, 0x80000000, x, y, xa, ya))

#define PrintTextA_str(s, font, cr, x, xa, y, ya, a) \
   DrawCallI(sm_LT, s, font, PrintTextArgs_A(cr, 0, x, y, xa, ya, a))

#define PrintTextF_str(s, font, cr, x, xa, y, ya, n) \
   DrawCallI(sm_LT, s, font, PrintTextArgs_F(cr, 0, x, y, xa, ya, n))

#define PrintTextFX_str(s, font, cr, x, xa, y, ya, n) \
   DrawCallI(sm_LT, s, font, PrintTextArgs_F(cr, 0x80000000, x, y, xa, ya, n))

#define PrintText(...)   PrintText_str  (PrintTextEnd(), __VA_ARGS__)
#define PrintTextX(...)  PrintTextX_str (PrintTextEnd(), __VA_ARGS__)
#define PrintTextA(...)  PrintTextA_str (PrintTextEnd(), __VA_ARGS__)
#define PrintTextF(...)  PrintTextF_str (PrintTextEnd(), __VA_ARGS__)
#define PrintTextFX(...) PrintTextFX_str(PrintTextEnd(), __VA_ARGS__)

#define SetClipArgs0(x, y) ((u32)(x) | ((u32)(y) << 16))
#define SetClipArgs1(w, h) SetClipArgs0(w, h)

#define SetClip(x, y, w, h) \
   DrawCallI(sm_LC, SetClipArgs0(x, y), SetClipArgs1(w, h))

#define SetClipW(x, y, w, h, ww) \
   DrawCallI(sm_LC, SetClipArgs0(x, y), SetClipArgs1(w, h), (u32)(ww))

#define ClearClip() \
   DrawCallI(sm_LC)

#define SetSize(w, h) \
   DrawCallI(sm_LZ, (i32)(w), (i32)(h))

#define SetFadeArgs(n, time, fade) \
   ((u32)(n) | ((u32)(time) << 6) | (k32_to_i32(fade) << 15))

#define SetFade(n, time, fade) \
   DrawCallI(sm_LF, SetFadeArgs(n, time, fade))

#define CheckFade(n) \
   DrawCallI(sm_LX, n)

#define HudMessageLog(...) \
   ( \
      HudMessage(__VA_ARGS__), \
      ACS_OptHudMessage(HUDMSG_NOTWITHFULLMAP|HUDMSG_NOTWITH3DVIEW|HUDMSG_NOTWITHOVERLAYMAP|HUDMSG_LOG, hid_log_throwaway, CR_UNTRANSLATED, 0, 0, TS), \
      ACS_EndHudMessage() \
   )

#define HudMessageF(font, ...) \
   ( \
      ACS_SetFont(font), \
      HudMessage(__VA_ARGS__) \
   )

#define HudMessageParams(flags, id, tr, x, y, hold, ...) \
   ( \
      ACS_OptHudMessage((flags) | HUDMSG_NOTWITHFULLMAP, id, tr, x, y, hold), \
      ACS_EndHudMessage(__VA_ARGS__) \
   )

#define HudMessagePlain(id, x, y, hold) \
   ( \
      ACS_OptHudMessage(HUDMSG_PLAIN | HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold), \
      ACS_EndHudMessage() \
   )

#define HudMessageFade(id, x, y, hold, fadetime) \
   ( \
      ACS_OptHudMessage(HUDMSG_FADEOUT | HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold), \
      ACS_EndHudMessage(fadetime) \
   )

#define HudMessageAlpha(id, x, y, hold, alpha) \
   ( \
      HudMessageParams(HUDMSG_ALPHA, id, CR_UNTRANSLATED, x, y, hold, alpha) \
   )

#define CrBlue   "\C[Lith_Blue]"
#define CrGreen  "\C[Lith_Green]"
#define CrRed    "\C[Lith_Red]"
#define CrPurple "\C[Lith_Purple]"
#define CrGrey   "\C[Lith_Grey]"

enum
{
   font_misaki_gothic, /* 美咲ゴシック */
   font_misaki_mincho, /* 美咲明朝 */
   font_jfdot_gothic,  /* JFドットk6x8 */
   font_num,
};

stkcall void DrawSprite(str name, i32 flags, i32 id, k32 x, k32 y, k32 hold);
stkcall void DrawSpriteX(str name, i32 flags, i32 id, k32 x, k32 y, k32 hold, k32 a1);
stkcall void DrawSpriteXX(str name, i32 flags, i32 id, k32 x, k32 y, k32 hold, k32 a1, k32 a2);
stkcall void DrawSpriteXXX(str name, i32 flags, i32 id, k32 x, k32 y, k32 hold, k32 a1, k32 a2, k32 a3);
stkcall void DrawSpritePlain(str name, i32 id, k32 x, k32 y, k32 hold);
stkcall void DrawSpriteAlpha(str name, i32 id, k32 x, k32 y, k32 hold, k32 alpha);
stkcall void DrawSpriteFade(str name, i32 id, k32 x, k32 y, k32 hold, k32 fadetime);
void HudMessage(cstr fmt, ...);

/* EOF */
