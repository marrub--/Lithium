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

i32 cr_blue;
i32 cr_green;
i32 cr_grey;
i32 cr_pink;
i32 cr_purple;
i32 cr_red;
i32 cr_yellow;

void Draw_GInit(void) {
   Str(blue,   sOBJ "Blue");
   Str(green,  sOBJ "Green");
   Str(grey,   sOBJ "Grey");
   Str(pink,   sOBJ "Pink");
   Str(purple, sOBJ "Purple");
   Str(red,    sOBJ "Red");
   Str(yellow, sOBJ "Yellow");
   cr_blue   = ServCallI(sm_FindFontColor, blue);
   cr_green  = ServCallI(sm_FindFontColor, green);
   cr_grey   = ServCallI(sm_FindFontColor, grey);
   cr_pink   = ServCallI(sm_FindFontColor, pink);
   cr_purple = ServCallI(sm_FindFontColor, purple);
   cr_red    = ServCallI(sm_FindFontColor, red);
   cr_yellow = ServCallI(sm_FindFontColor, yellow);
}

/* EOF */
