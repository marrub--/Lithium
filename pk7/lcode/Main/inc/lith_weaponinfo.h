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
   int ammotype; // this is mainly used for occluding calls
   __str class;
   int type;
} weaponinfo_t;

extern weaponinfo_t weaponinfo[weapon_max];

void Lith_GetWeaponType(struct player_s *p);

#endif

