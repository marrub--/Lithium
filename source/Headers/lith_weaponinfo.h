// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifndef lith_weaponinfo_h
#define lith_weaponinfo_h

#include "lith_weapons.h"

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
   int slot;
   int pclass;
   __str name;
   __str pickupsound;
   int defammotype;
   __str defammoclass;
   __str classname;
   int type;
   int flags;
} weaponinfo_t;

typedef struct invweapon_s
{
   weaponinfo_t const *info;
   bool owned;
   int autoreload;
   int ammotype;
   __str ammoclass;
   int magmax;
   int magcur;
   int ammomax;
   int ammocur;
} invweapon_t;

typedef struct weapondata_s
{
   int slot[SLOT_MAX];
   invweapon_t inv[weapon_max];
   invweapon_t *cur;
   invweapon_t *prev;
} weapondata_t;

#ifndef EXTERNAL_CODE
extern weaponinfo_t const weaponinfo[weapon_max];
#endif

#endif

