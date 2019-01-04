// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#include "common.h"

#include <stdio.h>

#define DrawSpriteBegin(name) \
   do { \
      while(name[0] == ':') \
      { \
         ACS_BeginPrint(); \
         ACS_PrintLocalized(name); \
         name = ACS_EndStrParam(); \
      } \
      ACS_SetFont(name); \
      ACS_BeginPrint(); \
      ACS_PrintChar('a'); \
      ACS_MoreHudMessage(); \
   } while(0)

stkcall
void DrawSprite(str name, i32 flags, i32 id, k32 x, k32 y, k32 hold)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(flags|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage();
}

stkcall
void DrawSpriteX(str name, i32 flags, i32 id, k32 x, k32 y, k32 hold, k32 a1)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(flags|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(a1);
}

stkcall
void DrawSpriteXX(str name, i32 flags, i32 id, k32 x, k32 y, k32 hold, k32 a1, k32 a2)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(flags|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(a1, a2);
}

stkcall
void DrawSpriteXXX(str name, i32 flags, i32 id, k32 x, k32 y, k32 hold, k32 a1, k32 a2, k32 a3)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(flags|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(a1, a2, a3);
}

stkcall
void DrawSpritePlain(str name, i32 id, k32 x, k32 y, k32 hold)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage();
}

stkcall
void DrawSpriteAlpha(str name, i32 id, k32 x, k32 y, k32 hold, k32 alpha)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_ALPHA|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(alpha);
}

stkcall
void DrawSpriteFade(str name, i32 id, k32 x, k32 y, k32 hold, k32 fadetime)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_FADEOUT|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(fadetime);
}

void HudMessage(char const *fmt, ...)
{
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   ACS_MoreHudMessage();
}

// EOF
