// Copyright © 2018 Graham Sanderson, all rights reserved.
#ifndef LITH_DRAWING_H
#define LITH_DRAWING_H

#define PrintSprite(name, x, xa, y, ya) \
   URANUS("_LPS", name, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)-1.0)

#define PrintSpriteA(name, x, xa, y, ya, a) \
   URANUS("_LPS", name, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)(a))

#define PrintSpriteF(name, x, xa, y, ya, n) \
   URANUS("_LPS", name, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)(-(n) - 2))

#define PrintTextFmt(...) StrParamBegin(__VA_ARGS__)
#define PrintTextStr(s) (ACS_BeginPrint(), ACS_PrintString(s))

#define PrintText(font, cr, x, xa, y, ya) \
   URANUS("_LPT", ACS_EndStrParam(), font, cr, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)-1.0)

#define PrintTextA(font, cr, x, xa, y, ya, a) \
   URANUS("_LPT", ACS_EndStrParam(), font, cr, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)(a))

#define PrintTextF(font, cr, x, xa, y, ya, n) \
   URANUS("_LPT", ACS_EndStrParam(), font, cr, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)(-(n) - 2))

#define SetClip(x, y, w, h) \
   URANUS("_LSC", (int)(x), (int)(y), (int)(w), (int)(h), 0)

#define SetClipW(x, y, w, h, ww) \
   URANUS("_LSC", (int)(x), (int)(y), (int)(w), (int)(h), (int)(ww))

#define ClearClip() \
   URANUS("_LSC", 0, 0, 0, 0, 0)

#define SetSize(w, h) \
   URANUS("_LHS", (int)(w), (int)(h))

#define SetFade(n, time, fade) \
   URANUS("_LSF", (int)(n), (int)(time), (fixed)(fade))

#define CheckFade(n) \
   URANUS("_LCF", n)

#endif
