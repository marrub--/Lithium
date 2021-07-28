/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Generalized HUD functions.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef p_hud_h
#define p_hud_h

#include "w_world.h"

enum {
   _ssld_fixed,
   _ssld_slide,
   _ssld_under,
   _ssld_max,
};

enum {
   _sfont_small,
   _sfont_mid,
   _sfont_italic,
   _sfont_max,
};

void HUD_WeaponSlots(i32 cr_one, i32 cr_two, i32 cr_many, i32 cr_cur, i32 _x, i32 _y);
void HUD_Score(cstr fmt, i96 scr, str font, i32 cr);
script void HUD_Log(i32 cr, i32 x, i32 yy);
void HUD_DrawHealth(i32 health, i32 x, i32 y, i32 cr, i32 cr_fade);

#endif
