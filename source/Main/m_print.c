// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"

#include <stdio.h>

#define DrawSpriteBegin(name) \
   do { \
      while(name[0] == ':') \
      { \
         ACS_BeginPrint(); \
         ACS_BeginPrint(); \
         ACS_PrintString("LITH"); \
         ACS_PrintString(name); \
         ACS_PrintLocalized(ACS_EndStrParam()); \
         name = ACS_EndStrParam(); \
      } \
      ACS_SetFont(name); \
      ACS_BeginPrint(); \
      ACS_PrintChar('a'); \
      ACS_MoreHudMessage(); \
   } while(0)

//
// DrawSprite
//
stkcall
void DrawSprite(__str name, int flags, int id, fixed x, fixed y, fixed hold)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(flags|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage();
}

//
// DrawSpriteX
//
stkcall
void DrawSpriteX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(flags|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(a1);
}

//
// DrawSpriteXX
//
stkcall
void DrawSpriteXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(flags|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(a1, a2);
}

//
// DrawSpriteXXX
//
stkcall
void DrawSpriteXXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2, fixed a3)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(flags|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(a1, a2, a3);
}

//
// DrawSpritePlain
//
stkcall
void DrawSpritePlain(__str name, int id, fixed x, fixed y, fixed hold)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage();
}

//
// DrawSpriteAlpha
//
stkcall
void DrawSpriteAlpha(__str name, int id, fixed x, fixed y, fixed hold, fixed alpha)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_ALPHA|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(alpha);
}

//
// DrawSpriteFade
//
stkcall
void DrawSpriteFade(__str name, int id, fixed x, fixed y, fixed hold, fixed fadetime)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_FADEOUT|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(fadetime);
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

// EOF
