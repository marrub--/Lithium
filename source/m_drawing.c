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

#include "common.h"
#include "p_player.h"
#include "w_world.h"

struct globalcolors globalcolors;

alloc_aut(0) stkcall
void Draw_Init(void) {
   #define GlobalCr(name) \
      globalcolors.name = ServCallI(sm_FindFontColor, so_##name);
   #include "m_drawing.h"
}

alloc_aut(0) stkcall
i32 Draw_GetCr(i32 n) {
   switch(n) {
   case 'a': return CR_BRICK;
   case 'b': return CR_TAN;
   case 'c': return CR_GREY;
   case 'd': return CR_GREEN;
   case 'e': return CR_BROWN;
   case 'f': return CR_GOLD;
   case 'g': return CR_RED;
   case 'h': return CR_BLUE;
   case 'i': return CR_ORANGE;
   case 'j': return CR_WHITE;
   case 'k': return CR_YELLOW;
   case 'l': return CR_UNTRANSLATED;
   case 'm': return CR_BLACK;
   case 'n': return CR_LIGHTBLUE;
   case 'o': return CR_CREAM;
   case 'p': return CR_OLIVE;
   case 'q': return CR_DARKGREEN;
   case 'r': return CR_DARKRED;
   case 's': return CR_DARKBROWN;
   case 't': return CR_PURPLE;
   case 'u': return CR_DARKGREY;
   case 'v': return CR_CYAN;
   case 'w': return CR_ICE;
   case 'x': return CR_FIRE;
   case 'y': return CR_SAPPHIRE;
   case 'z': return CR_TEAL;
   case _gcr_class: return pl.color;
#define GlobalCr(name) case _gcr_##name: return Cr(name);
#define GlobalCrH(name)
#include "m_drawing.h"
   }
   return CR_UNTRANSLATED;
}

alloc_aut(0) script ext("ACS") addr(lsc_drawcr)
i32 Sc_DrawCr(i32 cr) {
   return Draw_GetCr(cr);
}

/* EOF */
