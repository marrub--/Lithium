#ifndef LITH_WEAPONINFO_H
#define LITH_WEAPONINFO_H

#include "lith_weapons.h"

#define SLOT_MAX 8

enum
{
   AT_None,
   AT_Mag,
   AT_Ammo
};

typedef struct weaponinfo_s
{
   int slot;
   __str name;
   __str pickupsound;
   int defammotype;
   __str defammoclass;
   __str class;
   int type;
} weaponinfo_t;

typedef struct activeweapon_s
{
   weaponinfo_t const *info;
   int ammotype;
   __str ammoclass;
} activeweapon_t;

extern weaponinfo_t const weaponinfo[weapon_max];

void Lith_PlayerUpdateWeapon(struct player_s *p);

#endif

