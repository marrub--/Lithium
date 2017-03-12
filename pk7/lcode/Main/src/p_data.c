#include "lith_common.h"
#include "lith_player.h"
#include "lith_version.h"


//----------------------------------------------------------------------------
// Static Functions
//

static void Lith_GetArmorType(player_t *p);


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_ButtonPressed
//
bool Lith_ButtonPressed(player_t *p, int bt)
{
   return p->buttons & bt && !(p->old.buttons & bt);
}

//
// Lith_SetPlayerVelocity
//
bool Lith_SetPlayerVelocity(player_t *p, fixed velx, fixed vely, fixed velz, bool add, bool setbob)
{
   if(add)
      p->velx += velx, p->vely += vely, p->velz += velz;
   else
      p->velx = velx, p->vely = vely, p->velz = velz;
   
   return ACS_SetActorVelocity(p->tid, velx, vely, velz, add, setbob);
}

//
// Lith_ValidatePlayerTID
//
void Lith_ValidatePlayerTID(player_t *p)
{
   if(ACS_ActivatorTID() == 0)
      ACS_Thing_ChangeTID(0, p->tid = ACS_UniqueTID());
   else if(p->tid != ACS_ActivatorTID())
      p->tid = ACS_ActivatorTID();
}

//
// Lith_PlayerUpdateData
//
// Update all of the player's data.
//
[[__call("ScriptS")]]
void Lith_PlayerUpdateData(player_t *p)
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
   
   p->buttons = ACS_GetPlayerInput(-1, INPUT_BUTTONS);
   
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
// Lith_ResetPlayer
//
// Reset some things on the player when they spawn.
//
[[__call("ScriptS")]]
void Lith_ResetPlayer(player_t *p)
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
   Lith_ValidatePlayerTID(p);
   
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
   
   // Any linked lists on the player need to be initialized here.
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
   ACS_TakeInventory("Lith_SniperScopedToken",  999);
   ACS_TakeInventory("Lith_RifleBurstIter",     999);
   ACS_TakeInventory("Lith_RocketLoaded",       999);
   ACS_TakeInventory("Lith_RocketIterated",     999);
   ACS_TakeInventory("Lith_RocketAltMode",      999);
   ACS_TakeInventory("Lith_MissileReset",       999);
   
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
      p->log("> Lithium " Lith_Version " :: Compiled %S", __DATE__);
      
      if(ACS_GetCVar("__lith_debug_on"))
         p->logH("> player_t is %u bytes long!", sizeof(player_t) * 4);
      else
         p->logH("> Press \"%jS\" to open the menu.", "lith_k_opencbi");
      
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


//----------------------------------------------------------------------------
// Static Functions
//

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

// EOF
