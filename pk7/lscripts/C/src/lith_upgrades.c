#include "lith_common.h"
#include "lith_player.h"
#include "lith_upgrades.h"
#include "lith_hudid.h"
#include <math.h>

#define A(n) static void Upgr_##n##_Activate(player_t *p, upgrade_t *upgr)
#define D(n) static void Upgr_##n##_Deactivate(player_t *p, upgrade_t *upgr)
#define U(n) static void Upgr_##n##_Update(player_t *p, upgrade_t *upgr)

A(JetBooster); D(JetBooster); U(JetBooster);
A(ReflexWetw); D(ReflexWetw); U(ReflexWetw);
D(RifleModes); U(RifleModes);
A(lolsords); D(lolsords); U(lolsords);
U(Implying);

#undef A
#undef D
#undef U

// ---------------------------------------------------------------------------
// Data.
//

#define A(n) Upgr_##n##_Activate
#define D(n) Upgr_##n##_Deactivate
#define U(n) Upgr_##n##_Update

static upgradeinfo_t const upgrade_info[UPGR_MAX] = {
// { "Name-------", Cost------, Auto-, Callbacks... },
// Body
   { "JetBooster",  0         , true,  null, A(JetBooster), D(JetBooster), U(JetBooster) },
   { "ReflexWetw",  0         , true,  null, A(ReflexWetw), D(ReflexWetw), U(ReflexWetw) },
   { "CyberLegs",   900000    , false, null },
   { "ReactArmour", 3200200   , false, null },
   { "Splitter",    800000    , false, null },
// Weapons
   { "GaussShotty", 770430    , false, "ShotgunUpgr" },
   { "RifleModes",  340100    , false, null, null, D(RifleModes), U(RifleModes)},
   { "ChargeRPG",   850000    , false, null },
   { "PlasLaser",   1400000   , false, null },
   { "OmegaRail",   2600700   , false, null },
// Downgrades
   { "SeriousMode", 0         , false, null },
   { "RetroWeps",   0         , false, null },
   { "lolsords",    1000      , false, null, A(lolsords), D(lolsords), U(lolsords) },
// :v
   { "Implying",    0         , false, null, null, null, U(Implying) },
// { "ZharkovMode", -100      , false, null },
   { "TorgueMode",  800000000 , false, null },
};

#undef A
#undef D
#undef U

// ---------------------------------------------------------------------------
// Callbacks.
//

// --------------------------------------
// JetBooster
//

static
void Upgr_JetBooster_Activate(player_t *p, upgrade_t *upgr)
{
   p->scoremul -= 0.15;
}

static
void Upgr_JetBooster_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->scoremul += 0.15;
}

static
void Upgr_JetBooster_Update(player_t *p, upgrade_t *upgr)
{
   if(p->frozen)
      return;
   
   fixed grounddist = p->z - p->floorz;
   
   if(ButtonPressed(p, BT_SPEED) && grounddist > 16.0 && p->rocketcharge >= rocketcharge_max)
   {
      fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
      
      ACS_PlaySound(0, "player/rocketboost");
      ACS_GiveInventory("Lith_RocketBooster", 1);
      ACS_SetActorVelocity(0,
                           p->velx + (cosk(angle) * 16.0),
                           p->vely + (sink(angle) * 16.0),
                           10.0, false, true);
      
      p->rocketcharge = 0;
      p->leaped = false;
   }
}

// --------------------------------------
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

static
void Upgr_ReflexWetw_Activate(player_t *p, upgrade_t *upgr)
{
   p->scoremul -= 0.15;
   ACS_SetAirControl(0.77);
   ACS_SetActorPropertyFixed(0, APROP_Speed, 1.0);
}

static
void Upgr_ReflexWetw_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->scoremul += 0.15;
   ACS_SetAirControl(0.00390625); // why god
   ACS_SetActorPropertyFixed(0, APROP_Speed, 0.7);
}

static
void Upgr_ReflexWetw_Update(player_t *p, upgrade_t *upgr)
{
   if(p->frozen)
      return;
   
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
         ACS_SetActorVelocity(0, p->velx + (cosk(angle) * 32.0),
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
      ACS_SetActorVelocity(0, p->velx + (cosk(angle) * 4.0),
                              p->vely + (sink(angle) * 4.0),
                              12.0,
                           false, true);
      
      p->leaped = true;
   }
}

// --------------------------------------
// RifleModes
//

static
void Upgr_RifleModes_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->riflefiremode = 0;
}

static
void Upgr_RifleModes_Update(player_t *p, upgrade_t *upgr)
{
   if(p->weapontype == weapon_combatrifle && p->riflefiremode == rifle_firemode_burst)
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

// --------------------------------------
// lolsords
//

static
void Upgr_lolsords_Activate(player_t *p, upgrade_t *upgr)
{
   upgr->user_str[0] = p->weaponclass;
   ACS_GiveInventory("Lith_Sword", 1);
}

static
void Upgr_lolsords_Deactivate(player_t *p, upgrade_t *upgr)
{
   ACS_TakeInventory("Lith_Sword", 1);
   ACS_SetWeapon(upgr->user_str[0]);
}

static
void Upgr_lolsords_Update(player_t *p, upgrade_t *upgr)
{
   ACS_SetWeapon("Lith_Sword");
}

// --------------------------------------
// Implying
//

static
void Upgr_Implying_Update(player_t *p, upgrade_t *upgr)
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


// ---------------------------------------------------------------------------
// Scripts.
//

void Upgr_ToggleActive(player_t *p, upgrade_t *upgr)
{
   upgr->active = !upgr->active;
   
   if(upgr->active && upgr->info->Activate)
      upgr->info->Activate(p, upgr);
   else if(!upgr->active && upgr->info->Deactivate)
      upgr->info->Deactivate(p, upgr);
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

//
//
// ---------------------------------------------------------------------------

