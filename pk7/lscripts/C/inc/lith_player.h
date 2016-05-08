#ifndef LITH_PLAYER_H
#define LITH_PLAYER_H

#define MAX_PLAYERS 8

#include "lith_weapons.h"
#include "lith_cbi.h"
#include "lith_upgrades.h"
#include "lith_pdata.h"

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
   weaponf_combatrifle    = 1 << 2,
   weaponf_rocketlauncher = 1 << 3,
   weaponf_plasmarifle    = 1 << 4,
   weaponf_bfg9000        = 1 << 5
};

enum
{
   key_red_bit         = 0,
   key_yellow_bit      = 1,
   key_blue_bit        = 2,
   key_redskull_bit    = 3,
   key_yellowskull_bit = 4,
   key_blueskull_bit   = 5,
   key_red         = 1 << key_red_bit,
   key_yellow      = 1 << key_yellow_bit,
   key_blue        = 1 << key_blue_bit,
   key_redskull    = 1 << key_redskull_bit,
   key_yellowskull = 1 << key_yellowskull_bit,
   key_blueskull   = 1 << key_blueskull_bit,
};

// 7/4/2016: That's a lot of data!
// edit 9/4/2016: Holy shit, that's really a lot of data!
// edit 7/5/2016: JESUS TAKE THE WHEEL
typedef struct player_s
{
   // Status data
   bool active;
   bool dead;
   int tid;
   int number;
   int cameratid;
   
   fixed x, y, z;
   fixed velx, vely, velz;
   fixed floorz;
   float pitch, yaw, roll;
   
   fixed pitchv, yawv;
   fixed forwardv, sidev, upv;
   int buttons;
   
   int maxhealth;
   int health;
   int armor;
   
   fixed viewheight;
   
   // Score
   score_t score;
   score_t scoreaccum;
   int scoreaccumtime;
   
   // CBI
   cbi_t cbi;
   bip_t bip;
   
   // Upgrades
   bool upgrades_wasinit;
   upgrade_t upgrades[UPGR_MAX];
   
   // Statistics
   int weaponsheld;
   int secretsfound;
   
   long healthsum;
   long healthused;
   
   long armorsum;
   long armorused;
   
   score_t scoresum;
   score_t scoreused;
   
   // Type / class
   __str name;
   
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
   float addpitch;
   float addyaw;
   
   float bobpitch;
   float bobyaw;
   
   // Weapons
   bool scopetoken;
   bool lastscopetoken;
   int riflefiremode;
   struct dlist_s *hudstrstack;
   
   // Inventory
   bool berserk;
   int weapons;
   int keys;
   
   // Movement
   int slidecharge;
   int rocketcharge;
   bool leaped;
   
   // Miscellaneous
   int frozen;
} player_t;

extern player_t players[MAX_PLAYERS];

void Lith_GiveScore(player_t *p, score_t score);

#endif
