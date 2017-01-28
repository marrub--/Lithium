#ifndef LITH_PLAYER_H
#define LITH_PLAYER_H

#include "lith_cbi.h"
#include "lith_upgrades.h"
#include "lith_pdata.h"
#include "lith_sigil.h"
#include "lith_bip.h"
#include "lith_log.h"
#include "lith_list.h"
#include "lith_weaponinfo.h"

#define MAX_PLAYERS 8
#define HUDSTRS_MAX 20

#define ForPlayer() \
   for(int _piter = 0; _piter < MAX_PLAYERS; _piter++) \
      __with(player_t *p = &players[_piter];) \
         if(p->active)

#define Lith_GiveAllScore(score, nomul) \
   ForPlayer() \
      Lith_GiveScore(p, score, nomul)

#define LocalPlayer (&players[ACS_PlayerNumber()])
#define PlayerDiscount(n) (score_t)((n) * p->discount)

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
   fixed pitch, yaw;
   
   // Movement
   fixed velx, vely, velz;
   fixed pitchv, yawv;
   fixed forwardv, sidev, upv;
   fixed speedmul;
   fixed jumpboost;
   
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

// 7/4/2016: That's a lot of data!
// edit 9/4/2016: Holy shit, that's really a lot of data!
// edit 7/5/2016: JESUS TAKE THE WHEEL
// edit 3/1/2017: help
// edit 6/1/2017: there's so much data that I had to split it
// edit 23/1/2017: D E S T R O Y
typedef struct player_s
{
   // Status data
   bool active;
   bool dead;
   bool staticinit;
   bool reinit;
   
   int tid;
   int number;
   int cameratid;
   long ticks;
   loginfo_t loginfo;
   __str name;
   
   [[__anonymous]] player_delta_t cur;
   player_delta_t old;
   
   // State without delta
   float pitchf, yawf; // pitch/yaw in precalculated sane radian format
   int maxhealth;
   int maxarmor;
   fixed jumpheight;
   fixed viewheight;
   float discount;
   
   // Additive view
   float addpitch;
   float addyaw;
   
   float bobpitch;
   float bobyaw;
   
   float extrpitch;
   float extryaw;
   
   // 🌌 「÷」 0
   // sigil_t sigil;
   
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
   int itemsbought;
   int upgradesowned;
   
   long healthsum;
   long healthused;
   
   long armorsum;
   long armorused;
   
   score_t scoresum;
   score_t scoreused;
   
   int unitstravelled;
   
   // Inventory
   weapondata_t weapon;
   
   int riflefiremode;
   list_t hudstrlist;
   
   __str weaponclass;
   __str armorclass;
   
   int armortype;
   keycards_t keys;
} player_t;

extern player_t players[MAX_PLAYERS];

[[__optional_args(1)]]
void Lith_GiveScore(player_t *p, score_t score, bool nomul);
void Lith_TakeScore(player_t *p, score_t score);

#endif
