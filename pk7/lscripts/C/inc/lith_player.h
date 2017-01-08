#ifndef LITH_PLAYER_H
#define LITH_PLAYER_H

#include "lith_weapons.h"
#include "lith_cbi.h"
#include "lith_upgrades.h"
#include "lith_pdata.h"
#include "lith_sigil.h"
#include "lith_bip.h"

#define MAX_PLAYERS 8
#define LOG_MAX 10

#define Lith_ForPlayer() \
   for(player_t *p = &players[0]; p < &players[MAX_PLAYERS]; p++) \
      if(p->active)

enum
{
   armor_none,
   armor_bonus,
   armor_green,
   armor_blue,
   armor_unknown,
   armor_max
};

typedef struct player_delta_s
{
   // Status
   int health;
   int armor;
   score_t score;
   
   // Position
   fixed x, y, z;
   fixed floorz;
   float pitch, yaw, roll;
   
   // Movement
   fixed velx, vely, velz;
   fixed pitchv, yawv;
   fixed forwardv, sidev, upv;
   
   // Input
   int buttons;
   bool scopetoken;
   int frozen;
} player_delta_t;

typedef struct keycards_s
{
   bool redcard     : 1;
   bool yellowcard  : 1;
   bool bluecard    : 1;
   bool redskull    : 1;
   bool yellowskull : 1;
   bool blueskull   : 1;
} keycards_t;

typedef struct logdata_s
{
   __str info;
   int time;
} logdata_t;

// 7/4/2016: That's a lot of data!
// edit 9/4/2016: Holy shit, that's really a lot of data!
// edit 7/5/2016: JESUS TAKE THE WHEEL
// edit 3/1/2017: help
// edit 6/1/2017: there's so much data that I had to split it
typedef struct player_s
{
   // Status data
   bool active;
   bool dead;
   bool staticinit;
   int tid;
   int number;
   int cameratid;
   logdata_t logdata[LOG_MAX];
   struct dlist_s *log;
   
   [[__anonymous]] player_delta_t cur;
   player_delta_t old;
   
   // State without delta
   int maxhealth;
   fixed viewheight;
   fixed speedmul;
   
   // Additive view
   float addpitch;
   float addyaw;
   
   float bobpitch;
   float bobyaw;
   
   // 🌌 「÷」 0
   sigil_t sigil;
   
   // Score
   score_t scoreaccum;
   int scoreaccumtime;
   double scoremul;
   
   // CBI
   cbi_t cbi;
   bip_t bip;
   
   // Upgrades
   upgrades_t upgrades;
   
   // Statistics
   int weaponsheld;
   int secretsfound;
   int itemsbought;
   int upgradesowned;
   
   long healthsum;
   long healthused;
   
   long armorsum;
   long armorused;
   
   score_t scoresum;
   score_t scoreused;
   
   unsigned long long unitstravelled;
   
   // Type / class
   __str name;
   
   __str weaponclass;
   __str armorclass;
   
   int weapontype;
   int armortype;
   
   // Weapons
   int riflefiremode;
   struct dlist_s *hudstrstack;
   
   // Inventory
   bool berserk;
   keycards_t keys;
   int weapons;
   
   // Movement
   int slidecharge;
   int rocketcharge;
   bool leaped;
} player_t;

extern player_t players[MAX_PLAYERS];

void Lith_GiveScore(player_t *p, score_t score);
void Lith_TakeScore(player_t *p, score_t score);

#endif
