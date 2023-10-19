// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ ReactArmor upgrade.                                                      │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"

#define udata pl.upgrdata.reactarmor

static
struct {cstr abbr, full;} const ArmorNames[] = {
   "BUL", "Bullets",
   "ENE", "Energy",
   "FIR", "Fire",
   "XMG", "FireMagic",
   "MAG", "Magic",
   "MEL", "Melee",
   "SHR", "Shrapnel",
   "ICE", "Ice",
   "HZD", "Hazard",
};

static
void RA_Take(i32 n) {
   InvTake(strp(_l(OBJ "RA_Bullets"),   _p(n)), INT32_MAX);
   InvTake(strp(_l(OBJ "RA_Energy"),    _p(n)), INT32_MAX);
   InvTake(strp(_l(OBJ "RA_Fire"),      _p(n)), INT32_MAX);
   InvTake(strp(_l(OBJ "RA_FireMagic"), _p(n)), INT32_MAX);
   InvTake(strp(_l(OBJ "RA_Magic"),     _p(n)), INT32_MAX);
   InvTake(strp(_l(OBJ "RA_Melee"),     _p(n)), INT32_MAX);
   InvTake(strp(_l(OBJ "RA_Shrapnel"),  _p(n)), INT32_MAX);
   InvTake(strp(_l(OBJ "RA_Ice"),       _p(n)), INT32_MAX);
}

static
void RA_Give(cstr name, i32 n) {
   InvGive(strp(_l(OBJ "RA_"), _p(name), _p(n)), 1);
}

void Upgr_ReactArmor_Deactivate(void) {
   udata.activearmor = 0;

   RA_Take(1);
   RA_Take(2);
}

void Upgr_ReactArmor_Render(void) {
   if(udata.activearmor && CVarGetI(sc_hud_showarmorind)) {
      PrintSprite(sp_HUD_M_SplitLeft, pl.hudlpos+12,1, 226,2);
      BeginPrintStrN(ArmorNames[udata.activearmor - 1].abbr, 3);
      PrintText(sf_bigupper, CR_LIGHTBLUE, pl.hudlpos+32,1, 217,0, _u_no_unicode);
   }
}

script_str ext("ACS") addr(OBJ "RA_Give")
void Z_GiveRA(i32 num) {
   if(!get_bit(pl.upgrades[UPGR_ReactArmor].flags, _ug_active)) return;

   if(udata.activearmor != num + 1) {
      cstr name = ArmorNames[num].full;

      udata.activearmor = num + 1;

      RA_Take(1);
      RA_Take(2);

      AmbientSound(ss_player_lane_rarmor_mode, 1.0);

      P_LogH(3, tmpstr(sl_log_activearmor), name);

      if(get_bit(pl.upgrades[UPGR_ReactArmor2].flags, _ug_active)) {
         RA_Give(name, 2);
      } else {
         RA_Give(name, 1);
      }
   }
}

/* EOF */
