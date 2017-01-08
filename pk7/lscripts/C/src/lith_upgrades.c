#include "lith_common.h"
#include "lith_player.h"
#include "lith_upgrades.h"
#include "lith_hudid.h"
#include <math.h>


//----------------------------------------------------------------------------
// Static Functions
//

#define A(n) static void Upgr_##n##_Activate(player_t *p, upgrade_t *upgr);
#define D(n) static void Upgr_##n##_Deactivate(player_t *p, upgrade_t *upgr);
#define U(n) [[__call("ScriptS")]] static void Upgr_##n##_Update(player_t *p, upgrade_t *upgr);

                            U(JetBooster)
A(ReflexWetw) D(ReflexWetw) U(ReflexWetw)
A(CyberLegs)  D(CyberLegs)  U(CyberLegs)

              D(RifleModes) U(RifleModes)

A(7777777)    D(7777777)    U(7777777)
A(lolsords)   D(lolsords)   U(lolsords)

                            U(Implying)
A(UNCEUNCE)   D(UNCEUNCE)   U(UNCEUNCE)

#undef A
#undef D
#undef U


//----------------------------------------------------------------------------
// Static Objects
//

#define A(n) .Activate = Upgr_##n##_Activate
#define D(n) .Deactivate = Upgr_##n##_Deactivate
#define U(n) .Update = Upgr_##n##_Update

static upgradeinfo_t const upgrade_info[UPGR_MAX] = {
// {"Name-------", Cost------, Auto-, BIP-----------, UC_Cat-, Score, Callbacks...},
   {"JetBooster",  0         , true , "JetBooster",   UC_Body, -0.15, U(JetBooster)},
   {"ReflexWetw",  0         , true , "ReflexWetw",   UC_Body, -0.15, A(ReflexWetw), D(ReflexWetw), U(ReflexWetw)},
   {"CyberLegs",   900000    , false, "CyberLegs",    UC_Body,  0.00, A(CyberLegs),  D(CyberLegs),  U(CyberLegs)},
   {"ReactArmour", 3200200   , false, "Yh0",          UC_Body,  0.00},
   
   {"GaussShotty", 770430    , false, "ShotgunUpgr",  UC_Weap,  0.00},
   {"RifleModes",  340100    , false, "RifleUpgr",    UC_Weap,  0.00, D(RifleModes), U(RifleModes)},
   {"ChargeRPG",   850000    , false, "LauncherUpgr", UC_Weap,  0.00},
   {"PlasLaser",   1400000   , false, "PlasmaUpgr",   UC_Weap,  0.00},
   {"OmegaRail",   2600700   , false, "CannonUpgr",   UC_Weap,  0.00},
   
   {"TorgueMode",  800000000 , false, null,           UC_Extr,  0.00},
// {"RetroWeps",   9999990   , false, null,           UC_Extr,  0.00},
   {"7777777",     82354300  , false, null,           UC_Extr,  0.10, A(7777777),  D(7777777),  U(7777777)},
   {"lolsords",    1000000   , false, null,           UC_Extr,  0.20, A(lolsords), D(lolsords), U(lolsords)},
   
   {"Implying",    0         , false, null,           UC_Down,  0.20, U(Implying)},
   {"UNCEUNCE",    0         , false, null,           UC_Down,  0.30, A(UNCEUNCE), D(UNCEUNCE), U(UNCEUNCE)},
};

#undef A
#undef D
#undef U


//----------------------------------------------------------------------------
// Static Functions
//

//---------------------------------------
// JetBooster
//

[[__call("ScriptS")]]
static void Upgr_JetBooster_Update(player_t *p, upgrade_t *upgr)
{
   if(p->rocketcharge < rocketcharge_max)
      p->rocketcharge++;
   
   if(p->frozen) return;
   
   fixed grounddist = p->z - p->floorz;
   
   if(ButtonPressed(p, BT_SPEED) && grounddist > 16.0 && p->rocketcharge >= rocketcharge_max)
   {
      fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
      
      ACS_PlaySound(0, "player/rocketboost");
      ACS_GiveInventory("Lith_RocketBooster", 1);
      Lith_SetPlayerVelocity(p,
                           p->velx + (cosk(angle) * 16.0),
                           p->vely + (sink(angle) * 16.0),
                           10.0, false, true);
      
      p->rocketcharge = 0;
      p->leaped = false;
   }
}

//---------------------------------------
// ReflexWetw
//

[[__call("ScriptS")]]
static void DOOOOODGE(player_t *p)
{
   _Accum vh = p->viewheight;
   for(int i = 0; i < 20; i++)
   {
      _Accum mul = 1.0 - (sink(i / 40.0) * 0.6);
      ACS_SetActorPropertyFixed(0, APROP_ViewHeight, vh * mul);
      ACS_Delay(1);
   }
   ACS_SetActorPropertyFixed(0, APROP_ViewHeight, vh);
}

static void Upgr_ReflexWetw_Activate(player_t *p, upgrade_t *upgr)
{
   p->speedmul += 0.3;
}

static void Upgr_ReflexWetw_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->speedmul -= 0.3;
}

[[__call("ScriptS")]]
static void Upgr_ReflexWetw_Update(player_t *p, upgrade_t *upgr)
{
   if(p->slidecharge < slidecharge_max)
      p->slidecharge++;
   
   if(p->frozen) return;
   
   fixed grounddist = p->z - p->floorz;
   
   if(p->slidecharge >= slidecharge_max)
   {
      if(grounddist == 0.0)
         p->leaped = false;
      
      if(p->buttons & BT_SPEED &&
         (grounddist <= 16.0 || !p->upgrades[UPGR_JetBooster].active))
      {
         fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
         
         ACS_PlaySound(0, "player/slide");
         Lith_SetPlayerVelocity(p, p->velx + (cosk(angle) * 32.0),
                                 p->vely + (sink(angle) * 32.0),
                                 0,
                              false, true);
         
         DOOOOODGE(p);
         
         p->slidecharge = 0;
      }
   }
   
   if(ButtonPressed(p, BT_JUMP) &&
      !ACS_CheckInventory("Lith_RocketBooster") && !p->leaped &&
      ((grounddist <= 16.0 && p->slidecharge < slidecharge_max) || grounddist > 16.0))
   {
      fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
      
      ACS_PlaySound(0, "player/doublejump");
      Lith_SetPlayerVelocity(p, p->velx + (cosk(angle) * 4.0),
                              p->vely + (sink(angle) * 4.0),
                              12.0,
                           false, true);
      
      p->leaped = true;
   }
}

//---------------------------------------
// CyberLegs
//

static void Upgr_CyberLegs_Activate(player_t *p, upgrade_t *upgr)
{
   p->speedmul += 0.2;
   p->jumpboost += 0.5;
}

static void Upgr_CyberLegs_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->speedmul -= 0.2;
   p->jumpboost -= 0.5;
}

[[__call("ScriptS")]]
static void Upgr_CyberLegs_Update(player_t *p, upgrade_t *upgr)
{
   fixed absvel = absk(p->old.velz) * 10.0k;
   
   if(p->velz == 0 && absvel > 100)
   {
      // TODO: play landing sound from EYE
      for(fixed i = absvel; i >= 100; i -= 100)
      {
         int tid;
         int fuckyou = p->tid;
         ACS_SpawnForced("Lith_ExplodoBoots", p->x, p->y, p->z, tid = ACS_UniqueTID());
         ACS_SetActivator(tid);
         ACS_SetPointer(AAPTR_TARGET, fuckyou);
         ACS_SetActivator(fuckyou);
      }
   }
}

//---------------------------------------
// RifleModes
//

static void Upgr_RifleModes_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->riflefiremode = 0;
}

[[__call("ScriptS")]]
static void Upgr_RifleModes_Update(player_t *p, upgrade_t *upgr)
{
   if(p->weapontype == weapon_rifle && p->riflefiremode == rifle_firemode_burst)
   {
      ACS_Warp(p->cameratid, 0, 0, p->viewheight, 0,
               WARPF_NOCHECKPOSITION | WARPF_MOVEPTR |
               WARPF_COPYINTERPOLATION | WARPF_COPYPITCH);
      
      ACS_SetHudSize(320, 200);
      ACS_SetHudClipRect(40, 100, 240, 40);
      DrawSpritePlain("lgfx/RifleScope.png", hid_rifle_scope_img, 40.1, 100.1, TICSECOND);
      DrawSpritePlain("LITHCAM1", hid_rifle_scope_cam, 0.1, 0.1, TICSECOND);
      ACS_SetHudClipRect(0, 0, 0, 0);
   }
}

//---------------------------------------
// 7777777
//

static void Upgr_7777777_Activate(player_t *p, upgrade_t *upgr)
{
   ACS_SetActorPropertyFixed(0, APROP_Gravity, 0.0);
}

static void Upgr_7777777_Deactivate(player_t *p, upgrade_t *upgr)
{
   ACS_SetActorPropertyFixed(0, APROP_Gravity, 1.0);
}

[[__call("ScriptS")]]
static void Upgr_7777777_Update(player_t *p, upgrade_t *upgr)
{
   fixed vel = -2;
   if(p->velz > 0) vel += p->velz;
   Lith_SetPlayerVelocity(p, p->velx, p->vely, vel, false, true);
}

//---------------------------------------
// lolsords
//

static void Upgr_lolsords_Activate(player_t *p, upgrade_t *upgr)
{
   upgr->user_str[0] = p->weaponclass;
   ACS_GiveInventory("Lith_Sword", 1);
}

static void Upgr_lolsords_Deactivate(player_t *p, upgrade_t *upgr)
{
   ACS_TakeInventory("Lith_Sword", 1);
   ACS_SetWeapon(upgr->user_str[0]);
}

[[__call("ScriptS")]]
static void Upgr_lolsords_Update(player_t *p, upgrade_t *upgr)
{
   ACS_SetWeapon("Lith_Sword");
}

//---------------------------------------
// Implying
//

[[__call("ScriptS")]]
static void Upgr_Implying_Update(player_t *p, upgrade_t *upgr)
{
   static __str strings[] = {
      "\Cd" ">implying",
      "\Cd" ">doombabbies",
      "\Cd" ">implying",
      "\Cd" ">doom shitters",
      "\Cd" ">>>>>>>clip",
      "\Cd" ">implying",
      "\Cj" "Report and ignore.",
      "\Cj" "caleb when?",
      "\Cd" ">implying",
      "\Cd" ">",
      "\Cd" ">>>",
      "\Cd" ">>>>>>",
      "\Cj" "is this compatible with brutal doom?",
      "\Cd" ">>>>>>>>>",
      "\Cd" ">>>>>>>>>>>>",
      "\Cd" ">>>>>>>>>>>>>>>",
      "\Cq" "<",
   };
   
   static int const num_strings = sizeof(strings) / sizeof(*strings);
   static int const id_max = hid_implyingE - hid_implyingS;
   
   int id = upgr->user_int[0];
   for(int i = 0, n = ACS_Random(0, 40); i < n; i++)
   {
      id = ++id % id_max;
      
      HudMessageF("BIGFONT", "%S", strings[ACS_Random(0, num_strings - 1)]);
      HudMessageFade(hid_implyingE + id,
                     ACS_RandomFixed(0.0, 1.0),
                     ACS_RandomFixed(0.0, 1.0),
                     ACS_RandomFixed(0.1, 0.4),
                     0.1);
   }
   
   upgr->user_int[0] = id;
}


//---------------------------------------
// UNCEUNCE
//

static void Upgr_UNCEUNCE_Activate(player_t *p, upgrade_t *upgr)
{
   ACS_SetMusic("lmusic/Unce.flac");
}

static void Upgr_UNCEUNCE_Deactivate(player_t *p, upgrade_t *upgr)
{
   ACS_SetMusic("*");
   ACS_FadeTo(0, 0, 0, 0, 0);
}

[[__call("ScriptS")]]
static void Upgr_UNCEUNCE_Update(player_t *p, upgrade_t *upgr)
{
   fixed reeeed = ACS_Sin(p->ticks / 35.0) + 1.0 / 2.0;
   fixed greeen = ACS_Cos(p->ticks / 24.0) + 1.0 / 2.0;
   fixed bluuue = ACS_Sin(p->ticks / 13.0) + 1.0 / 2.0;
   
   ACS_FadeTo(reeeed * 255, greeen * 255, bluuue * 255, 0.6, TICSECOND);
   p->bobpitch += bluuue * 0.015;
   p->bobyaw   += greeen * 0.01;
}


//----------------------------------------------------------------------------
// External Functions
//

void Upgr_ToggleActive(player_t *p, upgrade_t *upgr)
{
   upgr->active = !upgr->active;
   
   if(upgr->active && upgr->info->Activate)
   {
      upgr->info->Activate(p, upgr);
      p->scoremul += upgr->info->scoreadd;
   }
   else if(!upgr->active && upgr->info->Deactivate)
   {
      upgr->info->Deactivate(p, upgr);
      p->scoremul -= upgr->info->scoreadd;
   }
}

void Upgr_SetOwned(player_t *p, upgrade_t *upgr)
{
   if(upgr->owned)
   {
      Log("Upgr_SetOwned: YOU CANNOT BUY THESE THINGS");
      return;
   }
   
   if(upgr->info->bipunlock)
      Lith_UnlockBIPPage(&p->bip, upgr->info->bipunlock);
   
   if(upgr->info->auto_activate)
      Upgr_ToggleActive(p, upgr);
   
   upgr->owned = true;
   p->upgradesowned++;
}

bool Upgr_CanBuy(player_t *p, upgrade_t *upgr)
{
   return !upgr->owned && (p->score - upgr->info->cost) >= 0;
}

void Upgr_Buy(player_t *p, upgrade_t *upgr)
{
   if(!Upgr_CanBuy(p, upgr))
   {
      Log("Upgr_Buy: YOU CANNOT BUY THESE THINGS");
      return;
   }
   
   Lith_TakeScore(p, upgr->info->cost);
   Upgr_SetOwned(p, upgr);
}

void Lith_PlayerInitUpgrades(player_t *p)
{
   for(int i = 0; i < UPGR_MAX; i++)
   {
      upgrade_t *upgr = &p->upgrades[i];
      memset(upgr, 0, sizeof(upgr));
      
      upgr->info = &upgrade_info[i];
      
      if(upgr->info->cost == 0)
         Upgr_SetOwned(p, upgr);
   }
}

void Lith_PlayerUpdateUpgrades(player_t *p)
{
   for(int i = 0; i < UPGR_MAX; i++)
   {
      upgrade_t *upgr = &p->upgrades[i];
      if(upgr->active && upgr->info->Update)
         upgr->info->Update(p, upgr);
   }
}

void Lith_PlayerDeinitUpgrades(player_t *p)
{
   for(int i = 0; i < UPGR_MAX; i++)
   {
      upgrade_t *upgr = &p->upgrades[i];
      if(upgr->active)
      {
         upgr->wasactive = true;
         Upgr_ToggleActive(p, upgr);
      }
   }
}

void Lith_PlayerReinitUpgrades(player_t *p)
{
   for(int i = 0; i < UPGR_MAX; i++)
   {
      upgrade_t *upgr = &p->upgrades[i];
      if(upgr->wasactive)
      {
         upgr->wasactive = false;
         Upgr_ToggleActive(p, upgr);
      }
   }
}

// EOF

