// Copyright © 2018 Alison Sanderson, all rights reserved.

// Not 1.0 / 35.0 or even 0.028 because ZDoom is stupid.
#define TICSECOND (0.029)
#define TS TICSECOND

#define PrintSprite(name, x, xa, y, ya) \
   DrawCallI(s"LS", name, (int)(x), (int)(y), (int)(xa), (int)(ya))

#define PrintSpriteA(name, x, xa, y, ya, a) \
   DrawCallI(s"LS", name, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)(a))

#define PrintSpriteF(name, x, xa, y, ya, n) \
   DrawCallI(s"LS", name, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)(-(n) - 2))

#define PrintTextFmt(...) StrParamBegin(__VA_ARGS__)
#define PrintTextStr(s) (ACS_BeginPrint(), ACS_PrintString(s))

#define PrintTextArgs_N(x, y, xa, ya) \
   (((uint)(x) << 14) | ((uint)(y) << 5) | ((uint)(xa) << 2) | (uint)(ya))

#define PrintTextArgs_A(x, y, xa, ya, a) \
   (PrintTextArgs_N(x, y, xa, ya) | 0x40000000 | ((uint)((fixed)(a) * 0x3F) << 24))

#define PrintTextArgs_F(x, y, xa, ya, n) \
   (PrintTextArgs_N(x, y, xa, ya) | (((uint)(n) + 1) << 24))

#define PrintText(font, cr, x, xa, y, ya) \
   DrawCallI(s"LT", ACS_EndStrParam(), font, cr, PrintTextArgs_N(x, y, xa, ya))

#define PrintTextX(font, cr, x, xa, y, ya) \
   DrawCallI(s"LT", ACS_EndStrParam(), font, cr, PrintTextArgs_N(x, y, xa, ya) | 0x80000000)

#define PrintTextA(font, cr, x, xa, y, ya, a) \
   DrawCallI(s"LT", ACS_EndStrParam(), font, cr, PrintTextArgs_A(x, y, xa, ya, a))

#define PrintTextF(font, cr, x, xa, y, ya, n) \
   DrawCallI(s"LT", ACS_EndStrParam(), font, cr, PrintTextArgs_F(x, y, xa, ya, n))

#define PrintTextFX(font, cr, x, xa, y, ya, n) \
   DrawCallI(s"LT", ACS_EndStrParam(), font, cr, PrintTextArgs_F(x, y, xa, ya, n) | 0x80000000)

#define SetClip(x, y, w, h) \
   DrawCallI(s"LC", (int)(x), (int)(y), (int)(w), (int)(h), 0)

#define SetClipW(x, y, w, h, ww) \
   DrawCallI(s"LC", (int)(x), (int)(y), (int)(w), (int)(h), (int)(ww))

#define ClearClip() \
   DrawCallI(s"LC", 0, 0, 0, 0, 0)

#define SetSize(w, h) \
   DrawCallI(s"LZ", (int)(w), (int)(h))

#define SetFade(n, time, fade) \
   DrawCallI(s"LF", (int)(n), (int)(time), (fixed)(fade))

#define CheckFade(n) \
   DrawCallI(s"LX", n)

#define HudMessageLog(...) \
   ( \
      HudMessage(__VA_ARGS__), \
      ACS_OptHudMessage(HUDMSG_NOTWITHFULLMAP|HUDMSG_NOTWITH3DVIEW|HUDMSG_NOTWITHOVERLAYMAP|HUDMSG_LOG, hid_log_throwaway, CR_UNTRANSLATED, 0, 0, TICSECOND), \
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

enum
{
   font_misaki_gothic, // 美咲ゴシック
   font_misaki_mincho, // 美咲明朝
   font_jfdot_gothic,  // JFドットk6x8
   font_num,
};

stkcall void DrawSprite(__str name, int flags, int id, fixed x, fixed y, fixed hold);
stkcall void DrawSpriteX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1);
stkcall void DrawSpriteXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2);
stkcall void DrawSpriteXXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2, fixed a3);
stkcall void DrawSpritePlain(__str name, int id, fixed x, fixed y, fixed hold);
stkcall void DrawSpriteAlpha(__str name, int id, fixed x, fixed y, fixed hold, fixed alpha);
stkcall void DrawSpriteFade(__str name, int id, fixed x, fixed y, fixed hold, fixed fadetime);
void HudMessage(__str fmt, ...);

// EOF
