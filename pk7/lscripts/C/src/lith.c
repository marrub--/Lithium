#include "lith_common.h"
#include "lith_player.h"
#include "lith_list.h"
#include "lith_hudid.h"
#include "lith_hud.h"
#include "lith_cbi.h"
#include <math.h>

// ---------------------------------------------------------------------------
// Data.
//

player_t players[MAX_PLAYERS];

struct weaponid { __str class; int type, flag; };
static struct weaponid const weaponids[] = {
   { "Lith_Pistol",          weapon_pistol,         weaponf_pistol         },
   { "Lith_Shotgun",         weapon_shotgun,        weaponf_shotgun        },
   { "Lith_CombatRifle",     weapon_combatrifle,    weaponf_combatrifle    },
   { "Lith_GrenadeLauncher", weapon_rocketlauncher, weaponf_rocketlauncher },
   { "Lith_PlasmaRifle",     weapon_plasmarifle,    weaponf_plasmarifle    },
   { "Lith_BFG9000",         weapon_bfg9000,        weaponf_bfg9000        }
};
static int const weaponids_max = sizeof(weaponids) / sizeof(*weaponids);

// ---------------------------------------------------------------------------
// Scripts.
//

//
// Lith_ResetPlayer
//
// Reset some things on the player when a new map starts.
//

[[__call("ScriptI")]]
static
void Lith_ResetPlayer(player_t *p)
{
   p->active = true;
   p->dead = false;
   ACS_Thing_ChangeTID(0, p->tid = ACS_UniqueTID());
   ACS_SetAirControl(0.77);
   
   ACS_SpawnForced("Lith_CameraHax", ACS_GetActorX(0), ACS_GetActorY(0), ACS_GetActorZ(0),
                                     p->cameratid = ACS_UniqueTID());
   ACS_SetCameraToTexture(p->cameratid, "LITHCAM1", 34);
   
   p->viewheight = ACS_GetActorViewHeight(0);
   
   if(!p->cbi.wasinit)
      Lith_PlayerInitCBI(p);
   
   if(!p->upgrades_wasinit)
      Lith_PlayerInitUpgrades(p);
   
   // pls not exit map with murder thingies out
   // is bad practice
   ACS_TakeInventory("Lith_PistolScopedToken", 999);
   ACS_TakeInventory("Lith_ShotgunScopedToken", 999);
   
   // i cri tears of pain for APROP_SpawnHealth
   if(!p->maxhealth)
      p->maxhealth = ACS_GetActorProperty(0, APROP_Health);
   
   if(p->hudstrstack)
   {
      DList_Free(p->hudstrstack);
      p->hudstrstack = null;
   }
   
   p->lastscopetoken = false;
   
   p->slidecharge = slidecharge_max;
   p->rocketcharge = rocketcharge_max;
   p->leaped = false;
   
   p->bobyaw = 0.0f;
   p->bobpitch = 0.0f;
   
   p->addyaw = 0.0f;
   p->addpitch = 0.0f;
   
   p->scoreaccum = 0;
}

//
// Lith_GetWeaponType
//
// Update information on what kind of weapons we have.
//

[[__call("ScriptI")]]
static
void Lith_GetWeaponType(player_t *p)
{
   p->weapontype = weapon_unknown;
   p->weapons = 0;
   
   for(int i = 0; i < weaponids_max; i++)
   {
      struct weaponid const *id = &weaponids[i];
      
      if(ACS_CheckInventory(id->class))
         p->weapons |= id->flag;
      
      if(p->weapontype == weapon_unknown && !ACS_StrICmp(p->weaponclass, id->class))
         p->weapontype = id->type;
   }
}

//
// Lith_GetArmorType
//
// Update information on what kind of armour we have.
//

[[__call("ScriptI")]]
static
void Lith_GetArmorType(player_t *p)
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
// Lith_PlayerRender
//
// Render the heads-up display.
//

[[__call("ScriptI")]]
static
void Lith_PlayerRender(player_t *p)
{
   if(p->lastscopetoken && !p->scopetoken)
   {
      if(p->hudstrstack)
      {
         DList_Free(p->hudstrstack);
         p->hudstrstack = null;
      }
      
      for(int i = hid_scope_clearS; i <= hid_scope_clearE; i++)
      {
         HudMessage("");
         HudMessagePlain(i, 0.0, 0.0, 0.0);
      }
   }
   else if(p->scopetoken && !p->lastscopetoken)
   {
      p->hudstrstack = DList_Create();
      
      for(int i = 0; i < hudstrstack_max; i++)
         DList_InsertBack(p->hudstrstack, (listdata_t){
            .str = StrParam("%x", Random(0x1000, 0x7FFF))
         });
   }
   
   if(p->scopetoken)
   {
      Lith_RenderHUDWaves(p);
      Lith_RenderHUDStringStack(p);
      
      ACS_SetActorProperty(0, APROP_RenderStyle, STYLE_Subtract);
      ACS_SetActorPropertyFixed(0, APROP_Alpha, ACS_GetCVarFixed("lith_weapons_scopealpha"));
   }
   else
   {
      ACS_SetActorProperty(0, APROP_RenderStyle, STYLE_Translucent);
      ACS_SetActorPropertyFixed(0, APROP_Alpha, ACS_GetCVarFixed("lith_weapons_alpha"));
   }
}

//
// Lith_PlayerMove
//
// Update movement of the player, like sliding and jumping.
//

[[__call("ScriptI")]]
static
void Lith_PlayerMove(player_t *p)
{
   fixed grounddist = p->z - p->floorz;
   
   if(p->slidecharge >= slidecharge_max)
   {
      if(grounddist == 0.0)
         p->leaped = false;
      
      if(p->buttons & BT_SPEED && (grounddist <= 16.0 || !p->upgrades[UPGR_JetBooster].active))
      {
         fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
         
         ACS_PlaySound(0, "player/slide");
         ACS_SetActorVelocity(0, p->velx + (ACS_Cos(angle) * 32.0), p->vely + (ACS_Sin(angle) * 32.0), 0, false, true);
         
         p->slidecharge = 0;
      }
   }
   
   if(ButtonPressed(p, BT_JUMP) &&
      !ACS_CheckInventory("Lith_RocketBooster") && !p->leaped &&
      ((grounddist <= 16.0 && p->slidecharge < slidecharge_max) || grounddist > 16.0))
   {
      fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
      
      ACS_PlaySound(0, "player/doublejump");
      ACS_SetActorVelocity(0, p->velx + (ACS_Cos(angle) * 4.0), p->vely + (ACS_Sin(angle) * 4.0), 12.0, false, true);
      
      p->leaped = true;
   }
}

//
// Lith_PlayerDamageBob
//
// Update view bobbing when you get damaged.
//

[[__call("ScriptI")]]
static
void Lith_PlayerDamageBob(player_t *p)
{
   if(!p->berserk && p->health < p->prevhealth)
   {
      float angle = RandomFloat(tau, -tau);
      float distance;
      
      distance = (p->prevhealth - p->health) / (float)p->maxhealth;
      distance *= 0.2f;
      distance *= (200 - p->armor) / 200.0f;
      
      p->bobyaw = sinf(angle) * distance;
      p->bobpitch = cosf(angle) * distance;
   }
   
   p->bobyaw = lerpf(p->bobyaw, 0.0f, 0.1f);
   p->bobpitch = lerpf(p->bobpitch, 0.0f, 0.1f);
}

//
// Lith_GiveSecretScore
//
// Give score to the player when they enter a secret area.
//

[[__call("ScriptI")]]
static
void Lith_GiveSecretScore(player_t *p, int mul)
{
   [[__call("ScriptS"), __extern("ACS")]]
   extern void Lith_UpdateScore(void);
   
   ACS_SetActivator(p->tid);
   ACS_GiveInventory("Lith_ScoreCount", 9000 * mul);
   Lith_UpdateScore();
   p->secretsfound++;
}

//
// Lith_PlayerView
//
// Update additive view.
//

[[__call("ScriptI")]]
static
void Lith_PlayerView(player_t *p)
{
   if(ACS_GetCVar("lith_player_damagebob"))
   {
      float bobmul = ACS_GetCVarFixed("lith_player_damagebobmul");
      p->addyaw = p->bobyaw * bobmul;
      p->addpitch = p->bobpitch * bobmul;
   }
}

//
// Lith_PlayerScore
//
// Update score accumulator.
//

[[__call("ScriptI")]]
static
void Lith_PlayerScore(player_t *p)
{
   if(!p->scoreaccumtime || p->score < p->prevscore)
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

[[__call("ScriptI")]]
static
void Lith_PlayerStats(player_t *p)
{
   if(p->health < p->prevhealth)
      p->healthused += p->prevhealth - p->health;
   else if(p->health > p->prevhealth && ACS_Timer() != 1)
      p->healthsum += p->health - p->prevhealth;
   
   if(p->armor < p->prevarmor)
      p->armorused += p->prevarmor - p->armor;
   else if(p->armor > p->prevarmor && ACS_Timer() != 1)
      p->armorsum += p->armor - p->prevarmor;
}

//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Callback scripts.
//

[[__call("ScriptI"), __script("Lightning")]]
void Lith_Lightning(void)
{
   if(ACS_Random(0, 1000000) == 666)
      ACS_Thing_Destroy(0, true, 0);
}

[[__call("ScriptI"), __script("Respawn")]]
void Lith_PlayerRespawn(void)
{
   Lith_ResetPlayer(&players[ACS_PlayerNumber()]);
}

[[__call("ScriptI"), __script("Death")]]
void Lith_PlayerDeath(void)
{
   player_t *p = &players[ACS_PlayerNumber()];
   p->dead = true;
   p->score = 0;
   p->cbi.open = false;
   p->upgrades_wasinit = false;
   
   for(int i = 0; i < UPGR_MAX; i++)
   {
      upgrade_t *upgr = &p->upgrades[i];
      
      if(upgr->active)
         Upgr_ToggleActive(p, upgr);
      
      upgr->owned = false;
   }
}

[[__call("ScriptI"), __script("Open")]]
void Lith_World(void)
{
   int maxsecrets = ACS_GetLevelInfo(LEVELINFO_TOTAL_SECRETS);
   int prevsecrets = 0;
   
   ACS_Delay(1);
   
   for(;;)
   {
      int secrets = ACS_GetLevelInfo(LEVELINFO_FOUND_SECRETS);
      
      if(secrets > prevsecrets)
         for(int i = 0; i < MAX_PLAYERS; i++)
            if(players[i].active)
               Lith_GiveSecretScore(&players[i], secrets - prevsecrets);
      
      prevsecrets = secrets;
      
      ACS_Delay(1);
   }
}

[[__call("ScriptI"), __script("Disconnect")]]
void Lith_PlayerDisconnect(void)
{
   players[ACS_PlayerNumber()].active = false;
}

[[__call("ScriptI"), __script("Enter")]]
void Lith_Player(void)
{
   player_t *p = &players[ACS_PlayerNumber()];
   
   Lith_ResetPlayer(p);
   
   while(p->active)
   {
      // -- Update data
      // Status data
      p->x = ACS_GetActorX(0);
      p->y = ACS_GetActorY(0);
      p->z = ACS_GetActorZ(0);
      p->velx = ACS_GetActorVelX(0);
      p->vely = ACS_GetActorVelY(0);
      p->velz = ACS_GetActorVelZ(0);
      p->yaw = ACS_GetActorAngle(0) - p->addyaw;
      p->pitch = ACS_GetActorPitch(0) - p->addpitch;
      p->pitchv = ACS_GetPlayerInputFixed(-1, INPUT_PITCH);
      p->yawv = ACS_GetPlayerInputFixed(-1, INPUT_YAW);
      p->roll = ACS_GetActorRoll(0);
      p->forwardv = ACS_GetPlayerInputFixed(-1, INPUT_FORWARDMOVE);
      p->sidev = ACS_GetPlayerInputFixed(-1, INPUT_SIDEMOVE);
      p->upv = ACS_GetPlayerInputFixed(-1, INPUT_UPMOVE);
      p->floorz = ACS_GetActorFloorZ(0);
      p->buttons = ACS_GetPlayerInput(-1, INPUT_BUTTONS);
      p->oldbuttons = ACS_GetPlayerInput(-1, INPUT_OLDBUTTONS);
      p->name = StrParam("%tS", 0);
      
      p->health = ACS_GetActorProperty(0, APROP_Health);
      p->armor = ACS_CheckInventory("BasicArmor");
      
      // This can be changed any time during script / playsim run, so save it here
      score_t curscore = p->score;
      
      // Type / class
      p->weaponclass = ACS_GetWeapon();
      p->armorclass = ACS_GetArmorInfoString(ARMORINFO_CLASSNAME);
      
      Lith_GetWeaponType(p);
      Lith_GetArmorType(p);
      
      // Inventory
      p->berserk = ACS_CheckInventory("PowerStrength");
      p->scopetoken = ACS_CheckInventory("Lith_ShotgunScopedToken") ||
         ACS_CheckInventory("Lith_PistolScopedToken");
      
      p->keys = 0;
      p->keys |= ACS_CheckInventory("RedCard")     << key_red_bit;
      p->keys |= ACS_CheckInventory("YellowCard")  << key_yellow_bit;
      p->keys |= ACS_CheckInventory("BlueCard")    << key_blue_bit;
      p->keys |= ACS_CheckInventory("RedSkull")    << key_redskull_bit;
      p->keys |= ACS_CheckInventory("YellowSkull") << key_yellowskull_bit;
      p->keys |= ACS_CheckInventory("BlueSkull")   << key_blueskull_bit;
      
      // Run scripts
      
      // -- Logic
      if(!p->dead)
      {
         Lith_PlayerStats(p);
         Lith_PlayerScore(p);
         Lith_PlayerDamageBob(p);
         
         Lith_PlayerUpdateCBI(p);
         
         if(p->rocketcharge < rocketcharge_max)
            p->rocketcharge++;
         
         if(p->slidecharge < slidecharge_max)
            p->slidecharge++;
         
         if(!p->frozen)
            Lith_PlayerMove(p);
         
         Lith_PlayerUpdateUpgrades(p);
         
         ACS_SetPlayerProperty(0, p->frozen > 0, PROP_TOTALLYFROZEN);
         Lith_PlayerView(p);
      }
      
      // -- Rendering
      Lith_PlayerHUD(p);
      Lith_PlayerRender(p);
      
      if(p->cbi.open)
         Lith_PlayerDrawCBI(p);
      
      // Update view
      ACS_SetActorAngle(0, ACS_GetActorAngle(0) - p->addyaw);
      ACS_SetActorPitch(0, ACS_GetActorPitch(0) - p->addpitch);
      
      // Tic passes
      ACS_Delay(1);
      
      // Update previous-tic values
      p->lastscopetoken = p->scopetoken;
      
      p->prevhealth = p->health;
      p->prevarmor = p->armor;
      p->prevscore = curscore;
      
      // Reset view for next tic
      ACS_SetActorAngle(0, ACS_GetActorAngle(0) + p->addyaw);
      ACS_SetActorPitch(0, ACS_GetActorPitch(0) + p->addpitch);
   }
}

//
// ---------------------------------------------------------------------------

