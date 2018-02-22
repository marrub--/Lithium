// Copyright © 2018 Graham Sanderson, all rights reserved.

// Not 1.0 / 35.0 or even 0.028 because ZDoom is stupid.
#define TICSECOND (0.029)
#define TS TICSECOND

#define PrintSprite(name, x, xa, y, ya) \
   URANUS("LPS", name, (int)(x), (int)(y), (int)(xa), (int)(ya))

#define PrintSpriteA(name, x, xa, y, ya, a) \
   URANUS("LPS", name, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)(a))

#define PrintSpriteF(name, x, xa, y, ya, n) \
   URANUS("LPS", name, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)(-(n) - 2))

#define PrintTextFmt(...) StrParamBegin(__VA_ARGS__)
#define PrintTextStr(s) (ACS_BeginPrint(), ACS_PrintString(s))

#define PrintText(font, cr, x, xa, y, ya) \
   URANUS("LPT", ACS_EndStrParam(), font, cr, (int)(x), (int)(y), (int)(xa), (int)(ya))

#define PrintTextA(font, cr, x, xa, y, ya, a) \
   URANUS("LPT", ACS_EndStrParam(), font, cr, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)(a))

#define PrintTextF(font, cr, x, xa, y, ya, n) \
   URANUS("LPT", ACS_EndStrParam(), font, cr, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)(-(n) - 2))

#define SetClip(x, y, w, h) \
   URANUS("LSC", (int)(x), (int)(y), (int)(w), (int)(h), 0)

#define SetClipW(x, y, w, h, ww) \
   URANUS("LSC", (int)(x), (int)(y), (int)(w), (int)(h), (int)(ww))

#define ClearClip() \
   URANUS("LSC", 0, 0, 0, 0, 0)

#define SetSize(w, h) \
   URANUS("LHS", (int)(w), (int)(h))

#define SetFade(n, time, fade) \
   URANUS("LSF", (int)(n), (int)(time), (fixed)(fade))

#define CheckFade(n) \
   URANUS("LCF", n)

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

[[__call("StkCall")]] void DrawSprite(__str name, int flags, int id, fixed x, fixed y, fixed hold);
[[__call("StkCall")]] void DrawSpriteX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1);
[[__call("StkCall")]] void DrawSpriteXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2);
[[__call("StkCall")]] void DrawSpriteXXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2, fixed a3);
[[__call("StkCall")]] void DrawSpritePlain(__str name, int id, fixed x, fixed y, fixed hold);
[[__call("StkCall")]] void DrawSpriteAlpha(__str name, int id, fixed x, fixed y, fixed hold, fixed alpha);
[[__call("StkCall")]] void DrawSpriteFade(__str name, int id, fixed x, fixed y, fixed hold, fixed fadetime);
void HudMessage(__str fmt, ...);

// EOF
