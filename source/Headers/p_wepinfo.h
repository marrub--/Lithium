// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifndef p_wepinfo_h
#define p_wepinfo_h

#include "p_weapons.h"

#define HasWeapon(p, w) (p)->weapon.inv[(w)].owned

#define SLOT_MAX 8

enum
{
   AT_None = 0,
   AT_NMag = 1 << 0,
   AT_Ammo = 1 << 1,
   AT_AMag = AT_NMag | AT_Ammo,
};

enum
{
   wf_magic = 1 << 0
};

typedef struct weaponinfo_s
{
   i32 slot;
   i32 pclass;
   str name;
   str pickupsound;
   i32 defammotype;
   str defammoclass;
   str classname;
   i32 type;
   i32 flags;
} weaponinfo_t;

typedef struct invweapon_s
{
   weaponinfo_t const *info;
   bool owned;
   i32 autoreload;
   i32 ammotype;
   str ammoclass;
   i32 magmax;
   i32 magcur;
   i32 ammomax;
   i32 ammocur;
} invweapon_t;

typedef struct weapondata_s
{
   i32 slot[SLOT_MAX];
   invweapon_t inv[weapon_max];
   invweapon_t *cur;
   invweapon_t *prev;
} weapondata_t;

extern weaponinfo_t const weaponinfo[weapon_max];

#endif
