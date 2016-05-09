#include "lith_common.h"
#include "lith_player.h"
#include "lith_upgrades.h"
#include "lith_hudid.h"
#include <math.h>

static void Upgr_JetBooster_Update(player_t *p, upgrade_t *upgr);
static void Upgr_RifleModes_Deactivate(player_t *p, upgrade_t *upgr);
static void Upgr_RifleModes_Update(player_t *p, upgrade_t *upgr);
static void Upgr_lolsords_Activate(player_t *p, upgrade_t *upgr);
static void Upgr_lolsords_Deactivate(player_t *p, upgrade_t *upgr);
static void Upgr_lolsords_Update(player_t *p, upgrade_t *upgr);
static void Upgr_Implying_Update(player_t *p, upgrade_t *upgr);

// ---------------------------------------------------------------------------
// Data.
//

static upgradeinfo_t const upgrade_info[UPGR_MAX] = {
// Body
   [UPGR_JetBooster]  = { 0         , true,  null, null, Upgr_JetBooster_Update },
   [UPGR_CyberLegs]   = { 900000    , false },
   [UPGR_ReactArmour] = { 3200200   , false },
   [UPGR_Splitter]    = { 800000    , false },
// Weapons
   [UPGR_GaussShotty] = { 770430    , false },
   [UPGR_RifleModes]  = { 340100    , false, null, Upgr_RifleModes_Deactivate, Upgr_RifleModes_Update },
   [UPGR_ChargeNader] = { 850000    , false },
   [UPGR_PlasLaser]   = { 1400000   , false },
   [UPGR_OmegaRail]   = { 2600700   , false },
// Downgrades
   [UPGR_SeriousMode] = { 0         , false },
   [UPGR_RetroWeps]   = { 0         , false },
   [UPGR_lolsords]    = { 1000      , false, Upgr_lolsords_Activate, Upgr_lolsords_Deactivate, Upgr_lolsords_Update },
// :v
   [UPGR_Implying]    = { 0         , false, null, null, Upgr_Implying_Update },
// [UPGR_ZharkovMode] = { -100      , false },
};

static __str upgrade_enums[] = {
   #define U(en, name) [UPGR_##en] = #en,
   #include "lith_upgradenames.h"
   null
};

__str upgrade_names[] = {
   #define U(en, name) [UPGR_##en] = name,
   #include "lith_upgradenames.h"
   null
};

// ---------------------------------------------------------------------------
// Callbacks.
//

// --------------------------------------
// JetBooster
//

static
void Upgr_JetBooster_Update(player_t *p, upgrade_t *upgr)
{
   fixed grounddist = p->z - p->floorz;
   
   if(ButtonPressed(p, BT_SPEED) && grounddist > 16.0 && p->rocketcharge >= rocketcharge_max)
   {
      fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
      
      ACS_PlaySound(0, "player/rocketboost");
      ACS_GiveInventory("Lith_RocketBooster", 1);
      ACS_SetActorVelocity(0, p->velx + (cosk(angle) * 16.0),
                              p->vely + (sink(angle) * 16.0),
                           10.0, false, true);
      
      p->rocketcharge = 0;
      p->leaped = false;
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
      HudMessageFade(hid_implyingE + id, ACS_RandomFixed(0.0, 1.0),
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
   
   p->score -= upgr->info->cost;
   Upgr_SetOwned(p, upgr);
}

void Lith_PlayerInitUpgrades(player_t *p)
{
   for(int i = 0; i < UPGR_MAX; i++)
   {
      upgrade_t *upgr = &p->upgrades[i];
      
      upgr->info = &upgrade_info[i];
      upgr->name = upgrade_names[i];
      upgr->description = Language("LITH_TXT_UPGRADE_%S", upgrade_enums[i]);
      
      if(upgr->info->cost == 0)
         Upgr_SetOwned(p, upgr);
   }
   
   p->upgrades_wasinit = true;
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

