#ifndef LITH_PLAYER_H
#define LITH_PLAYER_H

#define MAX_PLAYERS 8

#include "lith_weapons.h"

enum
{
   armor_none,
   armor_bonus,
   armor_green,
   armor_blue,
   armor_unknown,
   armor_max
};

enum
{
   weaponf_pistol         = 1 << 0,
   weaponf_shotgun        = 1 << 1,
   weaponf_supershotgun   = 1 << 2,
   weaponf_combatrifle    = 1 << 3,
   weaponf_rocketlauncher = 1 << 4,
   weaponf_plasmarifle    = 1 << 5,
   weaponf_bfg9000        = 1 << 6
};

enum
{
   key_red    = 1 << 0,
   key_yellow = 1 << 1,
   key_blue   = 1 << 2,
   key_red_bit    = 0,
   key_yellow_bit = 1,
   key_blue_bit   = 2
};

typedef long long int score_t;

// BIP - Biotic Information Panel
typedef struct bip_s
{
   void *no_data;
} bip_t;

// CBI - Computer/Brain Interface
typedef struct cbi_s
{
   bool open;
} cbi_t;

typedef struct player_s
{
   // Static
   int maxhealth;
   
   // Set every map
   int tid;
   
   // Status data
   bool active;
   
   fixed x, y, z;
   fixed velx, vely, velz;
   fixed floorz;
   float angle;
   float pitch;
   int buttons;
   
   int health;
   int armor;
   
   score_t score;
   score_t scoreaccum;
   int scoreaccumtime;
   
   cbi_t cbi;
   bip_t bip;
   
   // Type / class
   __str weaponclass;
   __str armorclass;
   
   int weapontype;
   int armortype;
   
   // From previous tic
   int oldbuttons;
   
   int prevhealth;
   int prevarmor;
   score_t prevscore;
   
   // Additive view
   float addangle;
   float addpitch;
   
   float bobangle;
   float bobpitch;
   
   // Misc. / inventory
   bool scopetoken;
   bool lastscopetoken;
   
   int riflefiremode;
   
   bool berserk;
   int weapons;
   int keys;
   
   int slidecharge;
   int rocketcharge;
   bool leaped;
   
   struct dlist_s *hudstrstack;
} player_t;

extern player_t players[MAX_PLAYERS];

#endif
