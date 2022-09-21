// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Weapon tracking information.                                             │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef p_weapons_h
#define p_weapons_h

#include "p_weaponinfo.h"

#define SLOT_MAX 8

enum {
   AT_None = 0,
   AT_NMag = dst_bit(0),
   AT_Ammo = dst_bit(1),
   AT_Mana = dst_bit(2),
   AT_AMag = AT_NMag | AT_Ammo,
   AT_MMag = AT_NMag | AT_Mana,
};

struct weaponinfo {
   i32  slot;
   i32  pclass;
   cstr name;
   str  pickupsound;
   i32  defammotype;
   str  defammoclass;
   str  classname;
   cstr typename;
};

struct invweapon {
   struct weaponinfo const *info;
   i32 type;
   str class;
   i32 ammotype;
   str ammoclass;
   i32 magmax;
   i32 magcur;
   i32 ammomax;
   i32 ammocur;
};

struct weapondata {
   i32 slot[SLOT_MAX];
   struct invweapon cur;
};

extern struct weaponinfo const weaponinfo[weapon_max];

i32 Wep_FromName(i32 name);

#endif
