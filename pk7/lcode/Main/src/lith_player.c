#include "lith_common.h"
#include "lith_player.h"
#include "lith_list.h"
#include "lith_hudid.h"
#include "lith_cbi.h"
#include "lith_version.h"
#include "lith_world.h"

#include <math.h>


//----------------------------------------------------------------------------
// Extern Objects
//

player_t players[MAX_PLAYERS];


//----------------------------------------------------------------------------
// Static Functions
//

[[__call("ScriptS")]] static void Lith_PlayerUpdateData(player_t *p);
[[__call("ScriptS")]] static void Lith_PlayerRunScripts(player_t *p);
[[__call("ScriptS")]] static void Lith_ResetPlayer(player_t *p);
static void Lith_GetArmorType(player_t *p);
[[__call("ScriptS")]] static void Lith_PlayerDamageBob(player_t *p);
[[__call("ScriptS")]] static void Lith_PlayerView(player_t *p);
static void Lith_PlayerStyle(player_t *p);
static void Lith_PlayerScore(player_t *p);
static void Lith_PlayerStats(player_t *p);
static void Lith_PlayerDeltaStats(player_t *p);
[[__call("ScriptS")]] static void Lith_PlayerHUD(player_t *p);

static void ValidateTID(player_t *p);
static void HUD_StringStack(player_t *p);
static void HUD_Waves(player_t *p);
static void HUD_Scope(player_t *p);


//----------------------------------------------------------------------------
// Scripts
//

//
// Lith_PlayerEntry
//
[[__call("ScriptS"), __script("Enter")]]
static void Lith_PlayerEntry(void)
{
   player_t *p = LocalPlayer;
   
reinit:
   while(!mapinit) ACS_Delay(1);
   
   Lith_ResetPlayer(p);
   Lith_PlayerLogEntry(p);
   Lith_PlayerEnterUpgrades(p);
   
   while(p->active)
   {
      if(p->reinit)
         goto reinit;
      
      Lith_PlayerUpdateData(p);
      
      // This can be changed any time, so save it here.
      player_delta_t olddelta = p->cur;
      
      // Run logic and rendering
      Lith_PlayerRunScripts(p);
      ACS_TakeInventory("Lith_AdrenalineToken", 1);
      
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
      
      // If the map changes this we need to make sure it's still correct.
      ValidateTID(p);
      
      p->ticks++;
   }
}

//
// Lith_PlayerDeath
//
[[__call("ScriptS"), __script("Death")]]
static void Lith_PlayerDeath(void)
{
   player_t *p = LocalPlayer;
   bool singleplayer = ACS_GameType() == GAME_SINGLE_PLAYER;
   
   p->dead = true;
   
   Lith_PlayerDeinitUpgrades(p);
   
   if(singleplayer || ACS_GetCVar("sv_cooploseinventory"))
   {
      Lith_PlayerLoseUpgrades(p);
      Lith_PlayerLoseBIPPages(&p->bip);
      p->score = p->scoreaccum = p->scoreaccumtime = 0;
   }
   
   if(singleplayer && ACS_GetCVar("lith_sv_revenge"))
   {
      ACS_LocalAmbientSound("player/death1", 127);
      ACS_Delay(35);
      ACS_GiveInventory("Lith_PlayerDeath", 1);
      ACS_Delay(25);
      ACS_GiveInventory("Lith_PlayerDeathNuke", 1);
   }
}

//
// Lith_PlayerRespawn
//
[[__call("ScriptS"), __script("Respawn")]]
static void Lith_PlayerRespawn(void)
{
   LocalPlayer->reinit = true;
}

//
// Lith_PlayerReturn
//
[[__call("ScriptS"), __script("Return")]]
static void Lith_PlayerReturn(void)
{
   LocalPlayer->reinit = true;
}

//
// Lith_PlayerDisconnect
//
[[__call("ScriptS"), __script("Disconnect")]]
static void Lith_PlayerDisconnect(void)
{
   player_t *p = LocalPlayer;
   
   Lith_DeallocateBIP(&p->bip);
   Lith_ListFree(&p->loginfo.hud);
   Lith_ListFree(&p->loginfo.full, free);
   Lith_ListFree(&p->hudstrlist, free);
   
   memset(p, 0, sizeof(player_t));
}


//----------------------------------------------------------------------------
// External Functions
//

void Lith_GiveScore(player_t *p, score_t score, bool nomul)
{
   // Could cause division by zero
   if(score == 0)
      return;
   
   // Multiply score by the player's multiplier
   if(!nomul)
      score *= p->scoremul;
   
   // Get a multiplier for the score accumulator and sound volume
   double mul = minmax(minmax(score, 0, 20000) / 20000.0f, 0.1f, 1.0f);
   double vol = 0.62 * mul;
   
   // Play a sound when we pick up score
   if(!IsSmallNumber(vol) && ACS_GetUserCVar(p->number, "lith_player_scoresound"))
      ACS_PlaySound(p->tid, "player/score", CHAN_ITEM, vol, false, ATTN_STATIC);
   
   //
   if(p->upgrades[UPGR_CyberLegs].active && ACS_Random(0, 10000) == 0)
      Lith_Log(p, "> You gained brouzouf.");
   
   if(p->upgrades[UPGR_TorgueMode].active && ACS_Random(0, 10) == 0)
      ACS_SpawnForced("Lith_EXPLOOOSION", p->x, p->y, p->z);
   
   // Add score and set score accumulator
   p->score          += score;
   p->scoresum       += score;
   p->scoreaccum     += score;
   p->scoreaccumtime += 20 * (mul * 2.0);
   
   // Log score
   if(ACS_GetUserCVar(p->number, "lith_player_scorelog"))
      Lith_LogH(p, "> +\Cj%lli\Cnscr", score);
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
      p->score      = 0;
      p->scoreused += delta;
   }
   
   p->scoreaccum     = 0;
   p->scoreaccumtime = 0;
}

[[__call("ScriptS")]]
void Lith_PlayerPayout(player_t *p)
{
#define Left(...) \
   ( \
      HudMessageF("CNFONT", __VA_ARGS__), \
      HudMessageParams(HUDMSG_FADEOUT, hid--, CR_WHITE, 16.1, y + .1, TICSECOND, .2) \
   )
      
#define Right(...) \
   ( \
      HudMessageF("CNFONT", __VA_ARGS__), \
      HudMessageParams(HUDMSG_FADEOUT, hid--, CR_WHITE, 280.2, y + .1, TICSECOND, .2) \
   )
   
#define Head(...) \
   ( \
      HudMessageF("DBIGFONT", __VA_ARGS__), \
      HudMessageParams(HUDMSG_FADEOUT, hid--, CR_WHITE, 8.1, y + 0.1, TICSECOND, 0.2) \
   )
   
#define GenCount(word, name) \
   if(1) \
   { \
      Left(word " %.1lk%%", pay.name##pct); \
      \
      if(i < 35) \
      { \
         HudMessageF("CNFONT", "%i\Cnscr", (int)lerplk(0, pay.name##scr, i / 34.0lk)); \
         HudMessageParams(HUDMSG_FADEOUT, hid, CR_WHITE, 280.2, y + 0.1, 2, 0.2); \
      } \
      \
      y += 9; \
      hid--; \
   } else (void)0
   
   payoutinfo_t pay = payout;
   
   ACS_SetActivator(p->tid);
   ACS_Delay(25);
   ACS_SetHudSize(320, 200);
   
   for(int i = 0; i < 35*3; i++)
   {
      int hid = hid_base_payout;
      int y = 16;
      bool counting = false;
      
      Head("RESULTS");
      
      if(i < 16)
      {
         HudMessageF("DBIGFONT", "RESULTS");
         HudMessageParams(HUDMSG_FADEOUT | HUDMSG_ADDBLEND, hid, CR_WHITE, 8 + 0.1, y + .1, TICSECOND, 0.5);
      }
      
      y += 16;
      hid--;
      
      if(pay.killmax) {GenCount("ELIMINATED", kill); counting |= pay.killnum;}
      if(pay.itemmax) {GenCount("ARTIFACTS",  item); counting |= pay.itemnum;}
      
      if(i > 35) {y += 7; Head("TOTAL"); y += 16;}
      if(i > 35 * 1.25) {Left("Tax"); Right("%i\Cnscr", pay.tax); y += 9;}
      
      if(i > 35 * 1.5)
      {
         Left("Total"); Right("%i\Cnscr", pay.total); y += 16;
         
         Head("PAYMENT"); y += 16;
         Left("Primary Account"); Right("%STRANSACTION CLOSED", (i % 6) == 0 ? "\Cn" : "");
      }
      
      if(ACS_GetUserCVar(p->number, "lith_player_resultssound"))
      {
         if(counting)
            if(i < 35)
               ACS_LocalAmbientSound("player/counter", 80);
            else if(i == 35)
               ACS_LocalAmbientSound("player/counterdone", 80);
         
         if(i == (int)(35 * 1.25) || i == (int)(35 * 1.5))
            ACS_LocalAmbientSound("player/counterdone", 80);
      }
      
      ACS_Delay(1);
   }
   
   ACS_Delay(20);
   
   Lith_GiveScore(p, pay.total, true);
   
#undef Left
#undef Right
#undef Head
#undef GenCount
}


//----------------------------------------------------------------------------
// Static Functions
//

//
// Lith_PlayerUpdateData
//
// Update all of the player's data.
//
[[__call("ScriptS")]]
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
   
   p->pitchf = (p->pitch - 0.5) * pi;
   p->yawf   = p->yaw * tau - pi;
   
   p->pitchv = ACS_GetPlayerInputFixed(-1, INPUT_PITCH);
   p->yawv   = ACS_GetPlayerInputFixed(-1, INPUT_YAW);
   
   p->forwardv = ACS_GetPlayerInputFixed(-1, INPUT_FORWARDMOVE);
   p->sidev    = ACS_GetPlayerInputFixed(-1, INPUT_SIDEMOVE);
   p->upv      = ACS_GetPlayerInputFixed(-1, INPUT_UPMOVE);
   
   p->buttons     = ACS_GetPlayerInput(-1, INPUT_BUTTONS);
   
   p->health = ACS_GetActorProperty(0, APROP_Health);
   p->armor  = ACS_CheckInventory("BasicArmor");
   
   p->name        = StrParam("%tS", p->number);
   p->weaponclass = ACS_GetWeapon();
   p->armorclass  = ACS_GetArmorInfoString(ARMORINFO_CLASSNAME);
   p->maxarmor    = ACS_GetArmorInfo(ARMORINFO_SAVEAMOUNT);
   
   Lith_GetArmorType(p);
   
   p->scopetoken = ACS_CheckInventory("Lith_CannonScopedToken") ||
                   ACS_CheckInventory("Lith_SniperScopedToken") ||
                   ACS_CheckInventory("Lith_ShotgunScopedToken") ||
                   ACS_CheckInventory("Lith_PistolScopedToken");
   
   p->keys.redcard     = ACS_CheckInventory("RedCard")    || ACS_CheckInventory("KeyGreen");
   p->keys.yellowcard  = ACS_CheckInventory("YellowCard") || ACS_CheckInventory("KeyYellow");
   p->keys.bluecard    = ACS_CheckInventory("BlueCard")   || ACS_CheckInventory("KeyBlue");
   p->keys.redskull    = ACS_CheckInventory("RedSkull");
   p->keys.yellowskull = ACS_CheckInventory("YellowSkull");
   p->keys.blueskull   = ACS_CheckInventory("BlueSkull");
}

//
// Lith_PlayerRunScripts
//
// Run main loop scripts.
//
[[__call("ScriptS")]]
static void Lith_PlayerRunScripts(player_t *p)
{
   // Data
   Lith_PlayerUpdateWeapon(p); // Update weapon info
   Lith_PlayerStats(p);        // Update statistics
   Lith_PlayerScore(p);        // Update score
   
   if(!p->dead)
   {
      // Logic
      Lith_PlayerUpdateCBI(p);      // Update CBI
      Lith_PlayerUpdateUpgrades(p); // Update upgrades
      Lith_PlayerDeltaStats(p);     // Update delta'd info
      Lith_PlayerUpdateLog(p);      // Update log data
   }
   
   // Rendering
   Lith_PlayerDamageBob(p);      // Update damage bobbing
   Lith_PlayerView(p);           // Update additive view
   Lith_PlayerRenderUpgrades(p); // Render Upgrades
   Lith_PlayerHUD(p);            // Draw HUD
   Lith_PlayerStyle(p);          // Change player render style
}

//
// Lith_ResetPlayer
//
// Reset some things on the player when they spawn.
//
[[__call("ScriptS")]]
static void Lith_ResetPlayer(player_t *p)
{
   //
   // Constant data
   
   p->active = true;
   p->reinit = p->dead = false;
   p->number = ACS_PlayerNumber();
   
   //
   // Map-static data
   
   memset(&p->old, 0, sizeof(player_delta_t));
   
   // If the map sets the TID on the first tic, it could already be set here.
   p->tid = 0;
   ValidateTID(p);
   
   // This keeps spawning more camera actors when you die, but that should be
   // OK as long as you don't die 2 billion times.
   ACS_SpawnForced("Lith_CameraHax", 0, 0, 0, p->cameratid = ACS_UniqueTID());
   ACS_SetCameraToTexture(p->cameratid, "LITHCAM1", 34);
   
   //
   // Reset data
   
   // i cri tears of pain for APROP_SpawnHealth
   if(!p->viewheight) p->viewheight = ACS_GetActorViewHeight(0);
   if(!p->jumpheight) p->jumpheight = ACS_GetActorPropertyFixed(0, APROP_JumpZ);
   if(!p->maxhealth)  p->maxhealth  = ACS_GetActorProperty(0, APROP_Health);
   if(!p->discount)   p->discount   = 1.0;
   
   Lith_ListFree(&p->loginfo.hud);
   Lith_ListFree(&p->hudstrlist, free);
   if(!p->loginfo.full.next) Lith_LinkDefault(&p->loginfo.full);
   if(!p->loginfo.maps.next) Lith_LinkDefault(&p->loginfo.maps);
   
   // pls not exit map with murder thingies out
   // is bad practice
   ACS_SetActorPropertyFixed(0, APROP_ViewHeight, p->viewheight);
   ACS_TakeInventory("Lith_PistolScopedToken",  999);
   ACS_TakeInventory("Lith_ShotgunScopedToken", 999);
   ACS_TakeInventory("Lith_CannonScopedToken",  999);
   
   Lith_PlayerResetCBI(p);
   
   p->frozen   = 0;
   
   p->addpitch = 0.0f;
   p->addyaw   = 0.0f;
   
   p->bobpitch = 0.0f;
   p->bobyaw   = 0.0f;
   
   p->extrpitch = 0.0f;
   p->extryaw   = 0.0f;
   
   p->scoreaccum     = 0;
   p->scoreaccumtime = 0;
   p->scoremul       = 1.3;
   
   //
   // Static data
   
   if(!p->staticinit)
   {
      Lith_PlayerInitBIP(p);
      Lith_PlayerInitUpgrades(p);
      Lith_Log (p, "> Lithium " Lith_Version " :: Compiled %S", __DATE__);
      Lith_LogH(p, "> Press \"%jS\" to open the menu.", "lith_k_opencbi");
      p->staticinit = true;
   }
   else
      Lith_PlayerReinitUpgrades(p);
   
   if(ACS_GetCVar("__lith_debug_on"))
   {
      p->score = 0xFFFFFFFFFFFFFFFFll;
      
      for(int i = weapon_min; i < weapon_max; i++)
         if(weaponinfo[i].class != null)
            ACS_GiveInventory(weaponinfo[i].class, 1);
   }
   
   Lith_UnlockBIPPage(&p->bip, "Pistol");
}

//
// Lith_GetArmorType
//
// Update information on what kind of armour we have.
//
static void Lith_GetArmorType(player_t *p)
{
#define Check(name) ACS_StrICmp(p->armorclass, #name) == 0
   
   if(Check(ArmorBonus))
      p->armortype = armor_bonus;
   else if(Check(GreenArmor) || Check(SilverShield))
      p->armortype = armor_green;
   else if(Check(BlueArmor) || Check(BlueArmorForMegasphere) || Check(EnchantedShield))
      p->armortype = armor_blue;
   else if(Check(None))
      p->armortype = armor_none;
   else
      p->armortype = armor_unknown;
   
#undef Check
}

//
// Lith_PlayerDamageBob
//
// Update view bobbing when you get damaged.
//
[[__call("ScriptS")]]
static void Lith_PlayerDamageBob(player_t *p)
{
   if(!ACS_CheckInventory("PowerStrength") && p->health < p->old.health)
   {
      float angle = RandomFloat(tau, -tau);
      float distance;
      
      distance = (p->old.health - p->health) / (float)p->maxhealth;
      distance *= 0.2f;
      distance *= (200 - p->armor) / 200.0f;
      
      p->bobyaw   = sinf(angle) * distance;
      p->bobpitch = cosf(angle) * distance;
   }
   
   p->bobpitch = lerpf(p->bobpitch, 0.0f, 0.1f);
   p->bobyaw   = lerpf(p->bobyaw,   0.0f, 0.1f);
}

//
// Lith_PlayerView
//
// Update additive view.
//
[[__call("ScriptS")]]
static void Lith_PlayerView(player_t *p)
{
   float addp = 0, addy = 0;
   
   if(ACS_GetUserCVar(p->number, "lith_player_damagebob"))
   {
      float bobmul = ACS_GetUserCVarFixed(p->number, "lith_player_damagebobmul");
      addp += p->bobpitch * bobmul;
      addy += p->bobyaw   * bobmul;
   }
   
   p->extrpitch = lerpf(p->extrpitch, 0.0f, 0.1f);
   p->extryaw   = lerpf(p->extryaw,   0.0f, 0.1f);
   
   p->addpitch = addp + p->extrpitch;
   p->addyaw   = addy + p->extryaw;
}

//
// Lith_PlayerStyle
//
static void Lith_PlayerStyle(player_t *p)
{
   if(p->scopetoken)
   {
      ACS_SetActorProperty(0, APROP_RenderStyle, STYLE_Subtract);
      ACS_SetActorPropertyFixed(0, APROP_Alpha, ACS_GetUserCVarFixed(p->number, "lith_weapons_scopealpha"));
   }
   else
   {
      ACS_SetActorProperty(0, APROP_RenderStyle, STYLE_Translucent);
      ACS_SetActorPropertyFixed(0, APROP_Alpha, ACS_GetUserCVarFixed(p->number, "lith_weapons_alpha"));
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
   if(p->frozen    != p->old.frozen)    ACS_SetPlayerProperty(0, p->frozen > 0, PROP_TOTALLYFROZEN);
   if(p->speedmul  != p->old.speedmul)  ACS_SetActorPropertyFixed(0, APROP_Speed, 0.7 + p->speedmul);
   if(p->jumpboost != p->old.jumpboost) ACS_SetActorPropertyFixed(0, APROP_JumpZ, p->jumpheight * (1 + p->jumpboost));
}

//
// Lith_PlayerHUD
//
[[__call("ScriptS")]]
static void Lith_PlayerHUD(player_t *p)
{
   ACS_SetHudSize(320, 200);
   HUD_Scope(p);
}

//
// ValidateTID
//
static void ValidateTID(player_t *p)
{
   if(ACS_ActivatorTID() == 0)
      ACS_Thing_ChangeTID(0, p->tid = ACS_UniqueTID());
   else if(p->tid != ACS_ActivatorTID())
      p->tid = ACS_ActivatorTID();
}

//
// HUD_StringStack
//
static void HUD_StringStack(player_t *p)
{
   typedef struct hudstr_s
   {
      __str str;
      list_t link;
   } hudstr_t;
   
   if((ACS_Timer() % 3) == 0)
   {
      hudstr_t *hudstr = calloc(1, sizeof(hudstr_t));
      Lith_LinkDefault(&hudstr->link, hudstr);
      hudstr->str = StrParam("%.8X", Random(0, 0x7FFFFFFF));
      
      Lith_ListLink(&p->hudstrlist, &hudstr->link);
      
      if(Lith_ListSize(&p->hudstrlist) == HUDSTRS_MAX)
         free(Lith_ListUnlink(p->hudstrlist.next));
   }
   
   ACS_SetHudSize(320, 200);
   ACS_SetFont("CONFONT");
   
   size_t i = 0;
   for(list_t *rover = p->hudstrlist.prev; rover != &p->hudstrlist; rover = rover->prev, i++)
   {
      hudstr_t *hudstr = rover->object;
      HudMessage("%S", hudstr->str);
      HudMessageParams(HUDMSG_ALPHA | HUDMSG_ADDBLEND, hid_scope_stringstackS - i, CR_RED, 300.2, 20.1 + (i * 9), 0.0, 0.5);
   }
}

//
// HUD_Waves
//
static void HUD_Waves(player_t *p)
{
   fixed health = (fixed)p->health / (fixed)p->maxhealth;
   int frame = minmax(health * 4, 1, 5);
   int timer = ACS_Timer();
   int pos;
   
   ACS_SetHudSize(320, 200);
   
   // Sine (health)
   pos = (10 + timer) % 160;
   DrawSpriteFade(StrParam("H_D1%i", frame),
      hid_scope_sineS - pos,
      300.1 + roundk(sink(pos / 32.0) * 7.0, 0),
      25.1 + pos,
      1.5, 0.3);
   
   // Square
   {
      fixed a = cosk(pos / 32.0);
      
      pos = (7 + timer) % 160;
      DrawSpriteFade(roundk(a, 2) != 0.0 ? "H_D16" : "H_D46",
         hid_scope_squareS - pos,
         300.1 + (a >= 0) * 7.0,
         25.1 + pos,
         1.9, 0.1);
   }
   
   // Triangle
   pos = (5 + timer) % 160;
   DrawSpriteFade("H_D14", hid_scope_triS - pos, 300.1 + abs((pos % 16) - 8), 25.1 + pos, 1.2, 0.2);
}

//
// HUD_Scope
//
static void HUD_Scope(player_t *p)
{
   if(p->old.scopetoken && !p->scopetoken)
   {
      Lith_ListFree(&p->hudstrlist, free);
      
      for(int i = hid_scope_clearS; i <= hid_scope_clearE; i++)
      {
         HudMessage("");
         HudMessagePlain(i, 0.0, 0.0, 0.0);
      }
   }
   
   if(p->scopetoken)
   {
      HUD_Waves(p);
      HUD_StringStack(p);
   }
}

// EOF

