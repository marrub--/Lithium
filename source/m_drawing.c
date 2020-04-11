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

#include "common.h"

struct globalcolors globalcolors;

void Draw_GInit(void) {
   #define GlobalCr(name) \
      Str(name, sOBJ #name); \
      globalcolors.name = ServCallI(sm_FindFontColor, name);
   #include "m_drawing.h"
}

/* EOF */
