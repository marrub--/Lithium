#ifndef LITH_WEAPONINFO_H
#define LITH_WEAPONINFO_H

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
   __str defmagclass;
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
   __str magclass;
   int magictake; // bullshit hack
} invweapon_t;

typedef struct weapondata_s
{
   int slot[SLOT_MAX];
   invweapon_t inv[weapon_max];
   invweapon_t *cur;
   invweapon_t *prev;
} weapondata_t;

extern weaponinfo_t const weaponinfo[weapon_max];

void Lith_PlayerUpdateWeapon(struct player *p);
void Lith_PlayerUpdateWeapons(struct player *p);

#endif

