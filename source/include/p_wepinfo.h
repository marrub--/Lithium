/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Weapon tracking information.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef p_wepinfo_h
#define p_wepinfo_h

#include "p_weapons.h"

#define HasWeapon(p, w) (p)->weapon.inv[(w)].owned

#define SLOT_MAX 8

enum {
   AT_None = 0,
   AT_NMag = dst_bit(0),
   AT_Ammo = dst_bit(1),
   AT_AMag = AT_NMag | AT_Ammo,
};

enum {
   wf_magic,
};

struct weaponinfo {
   i32 slot;
   i32 pclass;
   str name;
   str pickupsound;
   i32 defammotype;
   str defammoclass;
   str classname;
   i32 type;
   i32 flags;
};

struct invweapon {
   struct weaponinfo const *info;
   bool owned;
   i32 autoreload;
   i32 ammotype;
   str ammoclass;
   i32 magmax;
   i32 magcur;
   i32 ammomax;
   i32 ammocur;
};

struct weapondata {
   i32 slot[SLOT_MAX];
   struct invweapon inv[weapon_max];
   struct invweapon *cur;
   struct invweapon *prev;
};

extern struct weaponinfo const weaponinfo[weapon_max];
cstr P_Wep_GetPickup(i32 n);

#endif
