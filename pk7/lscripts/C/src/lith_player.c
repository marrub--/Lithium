#include "lith_common.h"
#include "lith_player.h"
#include "lith_list.h"
#include "lith_hudid.h"
#include "lith_hud.h"
#include "lith_cbi.h"
#include <math.h>


//----------------------------------------------------------------------------
// External Objects
//

player_t players[MAX_PLAYERS];


//----------------------------------------------------------------------------
// Static Objects
//

static __str const weaponclasses[weapon_max] = {
   [weapon_pistol]   = "Lith_Pistol",
   [weapon_shotgun]  = "Lith_Shotgun",
   [weapon_rifle]    = "Lith_CombatRifle",
   [weapon_launcher] = "Lith_GrenadeLauncher",
   [weapon_plasma]   = "Lith_PlasmaRifle",
   [weapon_bfg]      = "Lith_BFG9000",
};


//----------------------------------------------------------------------------
// Static Functions
//

static void Lith_PlayerUpdateData(player_t *p);
static void Lith_PlayerRunScripts(player_t *p);
static void Lith_ResetPlayer(player_t *p);
static void Lith_GetWeaponType(player_t *p);
static void Lith_GetArmorType(player_t *p);
static void Lith_PlayerDamageBob(player_t *p);
static void Lith_PlayerView(player_t *p);
static void Lith_PlayerScore(player_t *p);
static void Lith_PlayerStats(player_t *p);
static void Lith_PlayerDeltaStats(player_t *p);


//----------------------------------------------------------------------------
// Scripts
//

[[__call("ScriptS"), __script("Enter")]]
static void Lith_PlayerEntry(void)
{
   player_t *p = &players[ACS_PlayerNumber()];
   
   Lith_ResetPlayer(p);
   
   while(p->active)
   {
      Lith_PlayerUpdateData(p);
      
      // This can be changed any time, so save it here.
      player_delta_t olddelta = p->cur;
      
      Lith_PlayerRunScripts(p);
      
      // Update view
      ACS_SetActorAngle(0, ACS_GetActorAngle(0) - p->addyaw);
      ACS_SetActorPitch(0, ACS_GetActorPitch(0) - p->addpitch);
      
      // Tic passes
      ACS_Delay(1);
      
      // Update previous-tic values
      p->old = olddelta;
      
      // Reset view for next tic
      ACS_SetActorAngle(0, ACS_GetActorAngle(0) + p->addyaw);
      ACS_SetActorPitch(0, ACS_GetActorPitch(0) + p->addpitch);
      
      p->ticks++;
   }
}

[[__call("ScriptS"), __script("Death")]]
static void Lith_PlayerDeath(void)
{
   player_t *p = &players[ACS_PlayerNumber()];
   p->dead = true;
}

[[__call("ScriptS"), __script("Respawn")]]
static void Lith_PlayerRespawn(void)
{
   Lith_ResetPlayer(&players[ACS_PlayerNumber()]);
}

[[__call("ScriptS"), __script("Disconnect")]]
static void Lith_PlayerDisconnect(void)
{
   player_t *p = &players[ACS_PlayerNumber()];
   Lith_DeallocateBIP(&p->bip);
   if(p->log)         DList_Free(p->log);
   if(p->hudstrstack) DList_Free(p->hudstrstack);
   memset(p, 0, sizeof(player_t));
}

[[__call("ScriptS"), __script("Unloading")]]
static void Lith_PlayerUnloading(void)
{
   Lith_ForPlayer()
   {
      ACS_SetActivator(p->tid);
      Lith_PlayerDeinitUpgrades(p);
   }
}


//----------------------------------------------------------------------------
// External Functions
//

void Lith_GiveScore(player_t *p, score_t score)
{
   score *= p->scoremul;
   
   double mul = minmax(minmax(score, 0, 20000) / 20000.0f, 0.1f, 1.0f);
   
   if(p->upgrades[UPGR_CyberLegs].active && ACS_Random(0, 10000) == 0)
      Lith_Log(p, "You gained brouzouf.");
   
   if(ACS_GetUserCVar(p->number, "lith_player_scoresound") && mul > 0.1)
      ACS_PlaySound(p->tid, "player/score", CHAN_ITEM, 0.62f * mul, false, ATTN_STATIC);
   
   p->score          += score;
   p->scoresum       += score;
   p->scoreaccum     += score;
   p->scoreaccumtime += 35 * (mul * 2.0);
}

void Lith_TakeScore(player_t *p, score_t score)
{
   if(p->score - score >= 0)
   {
      p->score     -= score;
      p->scoreused += score;
   }
   else
   {
      score_t delta = p->score;
      p->score = 0;
      p->scoreused += delta;
   }
   
   p->scoreaccum     = 0;
   p->scoreaccumtime = 0;
}


//----------------------------------------------------------------------------
// Static Functions
//

//
// Lith_PlayerUpdateData
//
// Update all of the player's data.
//
static void Lith_PlayerUpdateData(player_t *p)
{
   p->x      = ACS_GetActorX(0);
   p->y      = ACS_GetActorY(0);
   p->z      = ACS_GetActorZ(0);
   p->floorz = ACS_GetActorFloorZ(0);
   
   p->velx = ACS_GetActorVelX(0);
   p->vely = ACS_GetActorVelY(0);
   p->velz = ACS_GetActorVelZ(0);
   
   p->pitch = ACS_GetActorPitch(0) - p->addpitch;
   p->yaw   = ACS_GetActorAngle(0) - p->addyaw;
   p->roll  = ACS_GetActorRoll(0);
   
   p->pitchv = ACS_GetPlayerInputFixed(-1, INPUT_PITCH);
   p->yawv   = ACS_GetPlayerInputFixed(-1, INPUT_YAW);
   
   p->forwardv = ACS_GetPlayerInputFixed(-1, INPUT_FORWARDMOVE);
   p->sidev    = ACS_GetPlayerInputFixed(-1, INPUT_SIDEMOVE);
   p->upv      = ACS_GetPlayerInputFixed(-1, INPUT_UPMOVE);
   
   p->buttons     = ACS_GetPlayerInput(-1, INPUT_BUTTONS);
   p->old.buttons = ACS_GetPlayerInput(-1, INPUT_OLDBUTTONS);
   
   p->health = ACS_GetActorProperty(0, APROP_Health);
   p->armor  = ACS_CheckInventory("BasicArmor");
   
   p->name        = StrParam("%tS", p->number);
   p->weaponclass = ACS_GetWeapon();
   p->armorclass  = ACS_GetArmorInfoString(ARMORINFO_CLASSNAME);
   p->maxarmor    = ACS_GetArmorInfo(ARMORINFO_SAVEAMOUNT);
   
   Lith_GetWeaponType(p);
   Lith_GetArmorType(p);
   
   p->berserk    = ACS_CheckInventory("PowerStrength");
   p->scopetoken = ACS_CheckInventory("Lith_ShotgunScopedToken") ||
                   ACS_CheckInventory("Lith_PistolScopedToken");
   
   p->keys.redcard     = ACS_CheckInventory("RedCard");
   p->keys.yellowcard  = ACS_CheckInventory("YellowCard");
   p->keys.bluecard    = ACS_CheckInventory("BlueCard");
   p->keys.redskull    = ACS_CheckInventory("RedSkull");
   p->keys.yellowskull = ACS_CheckInventory("YellowSkull");
   p->keys.blueskull   = ACS_CheckInventory("BlueSkull");
}

//
// Lith_PlayerRunScripts
//
// Run main loop scripts.
//
static void Lith_PlayerRunScripts(player_t *p)
{
   // Logic
   if(!p->dead)
   {
      Lith_PlayerStats(p);          // Update statistics
      Lith_PlayerScore(p);          // Update score
      Lith_PlayerUpdateCBI(p);      // Update CBI
      Lith_PlayerUpdateUpgrades(p); // Update Upgrades
      Lith_PlayerDeltaStats(p);     // Update delta'd info
   }
   
   // Rendering
   Lith_PlayerDamageBob(p); // Update damage bobbing
   Lith_PlayerHUD(p);       // Draw HUD
   Lith_PlayerView(p);      // Update additive view
}

//
// Lith_ResetPlayer
//
// Reset some things on the player when a new map starts.
//
static void Lith_ResetPlayer(player_t *p)
{
   //
   // Constant data
   
   p->active = true;
   p->dead = false;
   p->number = ACS_PlayerNumber();
   
   // i cri tears of pain for APROP_SpawnHealth
   if(!p->viewheight) p->viewheight = ACS_GetActorViewHeight(0);
   if(!p->jumpheight) p->jumpheight = ACS_GetActorPropertyFixed(0, APROP_JumpZ);
   if(!p->maxhealth)  p->maxhealth  = ACS_GetActorProperty(0, APROP_Health);
   
   //
   // Map-static data
   
   memset(&p->old, 0, sizeof(player_delta_t));
   ACS_Thing_ChangeTID(0, p->tid = ACS_UniqueTID());
   
   // This keeps spawning more camera actors when you die, but that should be
   // OK as long as you don't die 2 billion times.
   ACS_SpawnForced("Lith_CameraHax", 0, 0, 0, p->cameratid = ACS_UniqueTID());
   ACS_SetCameraToTexture(p->cameratid, "LITHCAM1", 34);
   
   //
   // Reset data
   
   if(p->log)
      DList_Free(p->log);
   
   p->log = DList_Create();
   
   if(p->hudstrstack)
   {
      DList_Free(p->hudstrstack);
      p->hudstrstack = null;
   }
   
   // pls not exit map with murder thingies out
   // is bad practice
   ACS_TakeInventory("Lith_PistolScopedToken",  999);
   ACS_TakeInventory("Lith_ShotgunScopedToken", 999);
   
   p->slidecharge  = slidecharge_max;
   p->rocketcharge = rocketcharge_max;
   p->leaped = false;
   p->cbi.open = false;
   p->frozen = 0;
   
   p->bobyaw = 0.0f;
   p->bobpitch = 0.0f;
   
   p->addyaw = 0.0f;
   p->addpitch = 0.0f;
   
   p->scoreaccum = 0;
   p->scoremul = 1.3;
   
   //
   // Static data
   
   if(!p->staticinit)
   {
      Lith_PlayerInitBIP(p);
      Lith_PlayerInitUpgrades(p);
      p->staticinit = true;
   }
   else
      Lith_PlayerReinitUpgrades(p);
   
   if(ACS_GetCVar("__lith_debug_on"))
   {
      p->score += 0xFFFFFFFFFFFFFFFFll;
      for(int i = weapon_min; i < weapon_max; i++)
         if(weaponclasses[i] != null)
            ACS_GiveInventory(weaponclasses[i], 1);
      
      for(int i = 0; i < UPGR_MAX; i++)
         if(!p->upgrades[i].owned)
            Upgr_SetOwned(p, &p->upgrades[i]);
      
      Lith_UnlockAllBIPPages(&p->bip);
   }
}

//
// Lith_GetWeaponType
//
// Update information on what kind of weapons we have.
//
static void Lith_GetWeaponType(player_t *p)
{
   p->weapontype = weapon_unknown;
   p->weapons = 0;
   for(int i = weapon_min; i < weapon_max; i++)
   {
      if(ACS_CheckInventory(weaponclasses[i])) p->weapons |= 1 << i;
      else                                     p->weapons &= ~(1 << i);
      
      if(p->weapontype == weapon_unknown && ACS_StrICmp(p->weaponclass, weaponclasses[i]) == 0)
         p->weapontype = i;
   }
}

//
// Lith_GetArmorType
//
// Update information on what kind of armour we have.
//
static void Lith_GetArmorType(player_t *p)
{
   static struct { __str class; int type; } const armorids[] = {
      { "None",                   armor_none  },
      { "ArmorBonus",             armor_bonus },
      { "GreenArmor",             armor_green },
      { "BlueArmor",              armor_blue  },
      { "BlueArmorForMegasphere", armor_blue  }
   };
   
   p->armortype = armor_unknown;
   for(int i = 0; i < sizeof(armorids) / sizeof(*armorids); i++)
      if(!ACS_StrICmp(p->armorclass, armorids[i].class))
      {
         p->armortype = armorids[i].type;
         break;
      }
}

//
// Lith_PlayerDamageBob
//
// Update view bobbing when you get damaged.
//
static void Lith_PlayerDamageBob(player_t *p)
{
   if(!p->berserk && p->health < p->old.health)
   {
      float angle = RandomFloat(tau, -tau);
      float distance;
      
      distance = (p->old.health - p->health) / (float)p->maxhealth;
      distance *= 0.2f;
      distance *= (200 - p->armor) / 200.0f;
      
      p->bobyaw   = sinf(angle) * distance;
      p->bobpitch = cosf(angle) * distance;
   }
   
   p->bobyaw   = lerpf(p->bobyaw,   0.0f, 0.1f);
   p->bobpitch = lerpf(p->bobpitch, 0.0f, 0.1f);
}

//
// Lith_PlayerView
//
// Update additive view.
//
static void Lith_PlayerView(player_t *p)
{
   if(ACS_GetUserCVar(p->number, "lith_player_damagebob"))
   {
      float bobmul = ACS_GetUserCVarFixed(p->number, "lith_player_damagebobmul");
      p->addpitch = p->bobpitch * bobmul;
      p->addyaw   = p->bobyaw   * bobmul;
   }
}

//
// Lith_PlayerScore
//
// Update score accumulator.
//
static void Lith_PlayerScore(player_t *p)
{
   if(!p->scoreaccumtime || p->score < p->old.score)
   {
      p->scoreaccum = 0;
      p->scoreaccumtime = 0;
   }
   
   if(p->scoreaccumtime > 0)
      p->scoreaccumtime--;
   else if(p->scoreaccumtime < 0)
      p->scoreaccumtime++;
}

//
// Lith_PlayerStats
//
// Update statistics.
//
static void Lith_PlayerStats(player_t *p)
{
   if(p->health < p->old.health)
      p->healthused += p->old.health - p->health;
   else if(p->health > p->old.health && ACS_Timer() != 1)
      p->healthsum += p->health - p->old.health;
   
   if(p->armor < p->old.armor)
      p->armorused += p->old.armor - p->armor;
   else if(p->armor > p->old.armor && ACS_Timer() != 1)
      p->armorsum += p->armor - p->old.armor;
   
   if(p->x != p->old.x) p->unitstravelled += abs(p->x - p->old.x);
   if(p->y != p->old.y) p->unitstravelled += abs(p->y - p->old.y);
   if(p->z != p->old.z) p->unitstravelled += abs(p->z - p->old.z);
}

//
// Lith_PlayerDeltaStats
//
static void Lith_PlayerDeltaStats(player_t *p)
{
   if(p->frozen != p->old.frozen)       ACS_SetPlayerProperty(0, p->frozen > 0, PROP_TOTALLYFROZEN);
   if(p->speedmul != p->old.speedmul)   ACS_SetActorPropertyFixed(0, APROP_Speed, 0.7 + p->speedmul);
   if(p->jumpboost != p->old.jumpboost) ACS_SetActorPropertyFixed(0, APROP_JumpZ, p->jumpheight * (1 + p->jumpboost));
}

// EOF

