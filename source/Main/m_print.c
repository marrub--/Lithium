// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"

#include <stdio.h>

#define DrawSpriteBegin(name) \
   do { \
      ACS_SetFont(name); \
      ACS_BeginPrint(); \
      ACS_PrintChar('a'); \
      ACS_MoreHudMessage(); \
   } while(0)

void (*DrawSprite)(__str name, int flags, int id, fixed x, fixed y, fixed hold);
void (*DrawSpriteX)(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1);
void (*DrawSpriteXX)(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2);
void (*DrawSpriteXXX)(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2, fixed a3);
void (*DrawSpritePlain)(__str name, int id, fixed x, fixed y, fixed hold);
void (*DrawSpriteAlpha)(__str name, int id, fixed x, fixed y, fixed hold, fixed alpha);
void (*DrawSpriteFade)(__str name, int id, fixed x, fixed y, fixed hold, fixed fadetime);

//
// A_DrawSprite
//
static void A_DrawSprite(__str name, int flags, int id, fixed x, fixed y, fixed hold)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(flags|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage();
}

//
// A_DrawSpriteX
//
static void A_DrawSpriteX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(flags|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(a1);
}

//
// A_DrawSpriteXX
//
static void A_DrawSpriteXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(flags|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(a1, a2);
}

//
// A_DrawSpriteXXX
//
static void A_DrawSpriteXXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2, fixed a3)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(flags|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(a1, a2, a3);
}

//
// A_DrawSpritePlain
//
static void A_DrawSpritePlain(__str name, int id, fixed x, fixed y, fixed hold)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage();
}

//
// A_DrawSpriteAlpha
//
static void A_DrawSpriteAlpha(__str name, int id, fixed x, fixed y, fixed hold, fixed alpha)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_ALPHA|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(alpha);
}

//
// A_DrawSpriteFade
//
static void A_DrawSpriteFade(__str name, int id, fixed x, fixed y, fixed hold, fixed fadetime)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_FADEOUT|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(fadetime);
}

#if 0
//
// Z_DrawSprite
//
static void Z_DrawSprite(__str name, int flags, int id, fixed x, fixed y, fixed hold)
{
}

//
// Z_DrawSpriteX
//
static void Z_DrawSpriteX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1)
{
}

//
// Z_DrawSpriteXX
//
static void Z_DrawSpriteXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2)
{
}

//
// Z_DrawSpriteXXX
//
static void Z_DrawSpriteXXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2, fixed a3)
{
}

//
// Z_DrawSpritePlain
//
static void Z_DrawSpritePlain(__str name, int id, fixed x, fixed y, fixed hold)
{
}

//
// Z_DrawSpriteAlpha
//
static void Z_DrawSpriteAlpha(__str name, int id, fixed x, fixed y, fixed hold, fixed alpha)
{
}

//
// Z_DrawSpriteFade
//
static void Z_DrawSpriteFade(__str name, int id, fixed x, fixed y, fixed hold, fixed fadetime)
{
}
#endif

//
// SetupSpriteBlitter
//
void SetupSpriteBlitter(void)
{
   // TODO: implement ZScript hudmessages
#if 0
   if(world.grafZoneEntered)
   {
      DrawSprite      = Z_DrawSprite;
      DrawSpriteX     = Z_DrawSpriteX;
      DrawSpriteXX    = Z_DrawSpriteXX;
      DrawSpriteXXX   = Z_DrawSpriteXXX;
      DrawSpriteAlpha = Z_DrawSpriteAlpha;
      DrawSpritePlain = Z_DrawSpritePlain;
      DrawSpriteFade  = Z_DrawSpriteFade;
   }
   else
#endif
   {
      DrawSprite      = A_DrawSprite;
      DrawSpriteX     = A_DrawSpriteX;
      DrawSpriteXX    = A_DrawSpriteXX;
      DrawSpriteXXX   = A_DrawSpriteXXX;
      DrawSpriteAlpha = A_DrawSpriteAlpha;
      DrawSpritePlain = A_DrawSpritePlain;
      DrawSpriteFade  = A_DrawSpriteFade;
   }
}

//
// HudMessage
//
void HudMessage(__str fmt, ...)
{
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);

   ACS_MoreHudMessage();
}

//
// HudMessageRainbows
//
void HudMessageRainbows(__str fmt, ...)
{
   static char const chars[] = {'g', 'i', 'k', 'd', 'h', 't', 'r'};
   va_list vl;

   ACS_BeginPrint();

   ACS_PrintChar('\C');
   ACS_PrintChar(chars[(ACS_Timer() / 4) % countof(chars)]);

   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);

   ACS_MoreHudMessage();
}

//
// Log
//
void Log(__str fmt, ...)
{
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);

   ACS_EndLog();
}

//
// PrintBold
//
void PrintBold(__str fmt, ...)
{
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);

   ACS_EndPrintBold();
}

// EOF
