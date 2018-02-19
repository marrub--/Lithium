// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_version.h"
#include "lith_world.h"
#include "lith_monster.h"

#include <math.h>

// Static Functions ----------------------------------------------------------|

//
// SetupAttributes
//
static void SetupAttributes(struct player *p)
{
   p->attr.names[at_acc] = "ACC";
   p->attr.names[at_def] = "DEF";
   p->attr.names[at_str] = "STR";
   p->attr.names[at_vit] = "VIT";
   p->attr.names[at_stm] = "STM";
   p->attr.names[at_luk] = "LUK";
   p->attr.names[at_rge] = "RGE";

   if(p->pclass & pcl_robot) {
      p->attr.names[at_vit] = "POT";
      p->attr.names[at_stm] = "REP";
   } else if(p->pclass & pcl_nonhuman) {
      p->attr.names[at_vit] = "POT";
      p->attr.names[at_stm] = "REG";
   }

   p->attr.expnext = 500;
   p->attr.level = 1;
}

//
// SetupInventory
//
static void SetupInventory(struct player *p)
{
   static container_t const baseinv[] = {
      {10, 14},
      {1, 3}, {1, 3}, {1, 3}, {1, 3},
      {4, 1},
      {2, 4}, {2, 4},
   };

   memmove(p->inv, baseinv, sizeof(baseinv));

   for(int i = 0; i < countof(p->inv); i++) {
      p->inv[i].items.construct();
      p->inv[i].user = p;
   }

   p->misc.items.construct();
   p->misc.user = p;
}

//
// SetPClass
//
static void SetPClass(struct player *p)
{
   __with(__str cl = p->pcstr = ACS_GetActorClass(0);) {
           if(cl == "Lith_MarinePlayer"   ) p->pclass = pcl_marine;
      else if(cl == "Lith_CyberMagePlayer") p->pclass = pcl_cybermage;
      else if(cl == "Lith_InformantPlayer") p->pclass = pcl_informant;
      else if(cl == "Lith_WandererPlayer" ) p->pclass = pcl_wanderer;
      else if(cl == "Lith_AssassinPlayer" ) p->pclass = pcl_assassin;
      else if(cl == "Lith_DarkLordPlayer" ) p->pclass = pcl_darklord;
      else if(cl == "Lith_ThothPlayer"    ) p->pclass = pcl_thoth;
      else if(cl == "FDPlutPlayer"  || cl == "FDTNTPlayer"    ||
              cl == "FDDoom2Player" || cl == "FDAliensPlayer" ||
              cl == "FDJPCPPlayer"  || cl == "FDBTSXPlayer")
         p->pclass = pcl_fdoomer;
      else if(cl == "DoomRLMarine"        || cl == "DoomRLScout"    ||
              cl == "DoomRLTechnician"    || cl == "DoomRLRenegade" ||
              cl == "DoomRLDemolitionist" || cl == "DoomRLCommando")
         p->pclass = pcl_drla;
      else for(;;)
      {
         Log("Invalid player class detected, everything is going to explode!");
         ACS_Delay(1);
      }
   }
}

//
// ConvertAmmo
//
[[__call("StkCall")]]
static void ConvertAmmo(struct player *p)
{
   int clip = InvNum("Lith_BulletAmmo");
   int shel = InvNum("Lith_ShellAmmo");
   int rckt = InvNum("Lith_RocketAmmo");
   int cell = InvNum("Lith_PlasmaAmmo");
   int bfgc = InvNum("Lith_CannonAmmo");

   InvTake("Lith_BulletAmmo", clip);
   InvTake("Lith_ShellAmmo",  shel);
   InvTake("Lith_RocketAmmo", rckt);
   InvTake("Lith_PlasmaAmmo", cell);
   InvTake("Lith_CannonAmmo", bfgc);

   if(p->pclass == pcl_fdoomer)
   {
      #define FDClass(cname) \
         if(p->pcstr == "FD" cname "Player") { \
            InvGive("FD" cname "Bullets",   clip); \
            InvGive("FD" cname "Shells",    shel); \
            InvGive("FD" cname "Rocket",    rckt); \
            InvGive("FD" cname "Cell",      cell); \
            InvGive("FD" cname "BFGCharge", bfgc); \
         }
      FDClass("Plut")
      FDClass("TNT")
      FDClass("Doom2")
      FDClass("Aliens")
      FDClass("JPCP")
      FDClass("BTSX")
      #undef FDClass
   }
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_PlayerCurWeaponType
//
[[__call("StkCall")]]
int Lith_PlayerCurWeaponType(struct player *p)
{
   return p->weapon.cur->info->type;
}

//
// Lith_ButtonPressed
//
[[__call("StkCall")]]
bool Lith_ButtonPressed(struct player *p, int bt)
{
   return p->buttons & bt && !(p->old.buttons & bt);
}

//
// Lith_SetPlayerVelocity
//
[[__call("StkCall")]]
bool Lith_SetPlayerVelocity(struct player *p, fixed velx, fixed vely, fixed velz, bool add)
{
   if(add)
      p->velx += velx, p->vely += vely, p->velz += velz;
   else
      p->velx = velx, p->vely = vely, p->velz = velz;

   return ACS_SetActorVelocity(p->tid, velx, vely, velz, add, true);
}

//
// Lith_ValidatePlayerTID
//
void Lith_ValidatePlayerTID(struct player *p)
{
   if(ACS_ActivatorTID() == 0) {
      ACS_Thing_ChangeTID(0, p->tid = ACS_UniqueTID());
      LogDebug(log_dev, "set ptid from 0 to %i", p->tid);
   } else if(p->tid != ACS_ActivatorTID()) {
      LogDebug(log_dev, "set ptid from %i to %i", p->tid, ACS_ActivatorTID());
      p->tid = ACS_ActivatorTID();
   }
}

//
// Lith_PlayerUpdateData
//
// Update all of the player's data.
//
[[__call("ScriptS")]]
void Lith_PlayerUpdateData(struct player *p)
{
   static int const warpflags = WARPF_NOCHECKPOSITION | WARPF_MOVEPTR |
      WARPF_WARPINTERPOLATION | WARPF_COPYINTERPOLATION | WARPF_COPYPITCH;

   HERMES("SetInput", p->num, false);

   ACS_Warp(p->cameratid,  4, 0, ACS_GetActorViewHeight(0), 0, warpflags);
   ACS_Warp(p->weathertid, 4, 0, ACS_GetActorViewHeight(0), 0, warpflags);

   p->x      = ACS_GetActorX(0);
   p->y      = ACS_GetActorY(0);
   p->z      = ACS_GetActorZ(0);
   p->floorz = ACS_GetActorFloorZ(0);

   p->velx = ACS_GetActorVelX(0);
   p->vely = ACS_GetActorVelY(0);
   p->velz = ACS_GetActorVelZ(0);

   p->pitch = ACS_GetActorPitch(0) - p->addpitch;
   p->yaw   = ACS_GetActorAngle(0) - p->addyaw;
   p->roll  = ACS_GetActorRoll (0) - p->addroll;

   p->pitchf = ((-p->pitch + 0.25) * 2) * pi;
   p->yawf   = p->yaw * tau - pi;

   p->pitchv = ACS_GetPlayerInputFixed(-1, INPUT_PITCH);
   p->yawv   = ACS_GetPlayerInputFixed(-1, INPUT_YAW);

   p->forwardv = ACS_GetPlayerInputFixed(-1, INPUT_FORWARDMOVE);
   p->sidev    = ACS_GetPlayerInputFixed(-1, INPUT_SIDEMOVE);
   p->upv      = ACS_GetPlayerInputFixed(-1, INPUT_UPMOVE);

   p->buttons = ACS_GetPlayerInput(-1, INPUT_BUTTONS);

   p->name        = StrParam("%tS", p->num);
   p->weaponclass = ACS_GetWeapon();

   p->scopetoken = InvNum("Lith_WeaponScopedToken");

   p->keys.rc = InvNum("RedCard")    ||
                InvNum("KeyGreen");
   p->keys.yc = InvNum("YellowCard") ||
                InvNum("KeyYellow");
   p->keys.bc = InvNum("BlueCard")   ||
                InvNum("KeyBlue");
   p->keys.rs = InvNum("RedSkull");
   p->keys.ys = InvNum("YellowSkull");
   p->keys.bs = InvNum("BlueSkull");

   if(p->pclass & pcl_mods)
      ConvertAmmo(p);

   DebugStat("attr points: %u\nexp: lv.%u %lu/%lu\n",
      p->attr.points, p->attr.level, p->attr.exp, p->attr.expnext);
   DebugStat("x: %k\ny: %k\nz: %k\n", p->x, p->y, p->z);
   DebugStat("vx: %k\nvy: %k\nvz: %k\nvel: %k\n", p->velx, p->vely, p->velz, p->getVel());
}

//
// Lith_GiveMail
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_GiveMail(int num)
{
   static __str const names[] = {
      "Intro",
      "Cluster1",
      "Cluster2",
      "Cluster3",
      "Phantom",
      "JamesDefeated",
      "MakarovDefeated",
      "IsaacDefeated"
   };

   num %= countof(names);

   withplayer(LocalPlayer)
      p->deliverMail(names[num]);
}

//
// Lith_ClearTextBuf
//
[[__call("StkCall")]]
void Lith_ClearTextBuf(struct player *p)
{
   memset(p->txtbuf, 0, sizeof(p->txtbuf));
   p->tbptr = 0;
}

//
// Lith_KeyDown
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_KeyDown(int pnum, int ch)
{
   withplayer(&players[pnum])
      if(p->tbptr + 1 < countof(p->txtbuf))
         p->txtbuf[p->tbptr++] = ch;
}

//
// Lith_GiveEXP
//
[[__call("StkCall")]]
void Lith_GiveEXP(struct player *p, u64 amt)
{
   #pragma GDCC FIXED_LITERAL OFF
   struct player_attributes *a = &p->attr;

   while(a->exp + amt >= a->expnext) {
      a->level++;
      a->points  += 5;
      a->expnext  = 500 + (a->level * pow(1.385, a->level * 0.2) * 340);
      p->attr.sup = p->attr.cur;
   }

   a->exp += amt;
}

//
// Lith_ResetPlayer
//
// Reset some things on the player when they spawn.
//
[[__call("ScriptS")]]
void Lith_ResetPlayer(struct player *p)
{
   //
   // Zero-init

   if(!p->wasinit) {
      *p = (struct player){};
      p->wasinit = true;
   }

   //
   // Constant data

   p->active = true;
   p->reinit = p->dead = false;
   p->num    = ACS_PlayerNumber();
   p->bipptr = &p->bip;

   //
   // Static data (pre-init)

   if(!p->staticinit)
   {
      SetPClass(p);
      SetupAttributes(p);
      SetupInventory(p);

      // i cri tears of pain for APROP_SpawnHealth
      p->viewheight   = ACS_GetActorViewHeight(0);
      p->jumpheight   = ACS_GetActorPropertyFixed(0, APROP_JumpZ);
      p->spawnhealth  = ACS_GetActorProperty(0, APROP_Health);
      p->spawndfactor = ACS_GetActorPropertyFixed(0, APROP_DamageFactor);
      p->maxhealth    = p->spawnhealth;
      p->discount     = 1.0;
      p->stepnoise    = StrParam("player/%S/step", p->classname);

      switch(ACS_GetPlayerInfo(p->num, PLAYERINFO_GENDER))
      {
      case 0: p->pronoun = pro_male;   break;
      case 1: p->pronoun = pro_female; break;
      case 2: p->pronoun = pro_object; break;
      }

      for(int i = 0; i < countof(p->notes); i++)
         p->notes[i] = "";
   }

   //
   // Map-static data

   p->old = (struct player_delta){};

   // If the map sets the TID on the first tic, it could already be set here.
   p->tid = 0;
   p->validateTID();

   // This keeps spawning more camera actors when you die, but that should be
   // OK as long as you don't die 2 billion times.
   ACS_SpawnForced("Lith_CameraHax", 0, 0, 0, p->cameratid  = ACS_UniqueTID());
   ACS_SpawnForced("Lith_CameraHax", 0, 0, 0, p->weathertid = ACS_UniqueTID());

   if(world.dbgScore)
      p->score = 0xFFFFFFFFFFFFFFFFll;

   //
   // Reset data

   // Any linked lists on the player need to be initialized here.
   p->loginfo.hud.free();
   p->hudstrlist.free(true);
   if(!p->loginfo.full.next) p->loginfo.full.construct();
   if(!p->loginfo.maps.next) p->loginfo.maps.construct();

   // pls not exit map with murder thingies out
   // is bad practice
   ACS_SetPlayerProperty(0, false, PROP_INSTANTWEAPONSWITCH);
   ACS_SetActorPropertyFixed(0, APROP_ViewHeight, p->viewheight);
   InvTake("Lith_WeaponScopedToken", 999);

   Lith_PlayerResetCBIGUI(p);

   p->frozen     = 0;
   p->semifrozen = 0;

   p->addpitch = 0.0f;
   p->addyaw   = 0.0f;
   p->addroll  = 0.0f;

   p->bobpitch = 0.0f;
   p->bobyaw   = 0.0f;

   p->extrpitch = 0.0f;
   p->extryaw   = 0.0f;

   p->scoreaccum     = 0;
   p->scoreaccumtime = 0;
   p->scoremul       = 1.3;

   p->alpha = 1;

   //
   // Re-init data

   if(!p->bip.init)
      Lith_PlayerInitBIP(p);

   if(!p->upgrinit)
      Lith_PlayerInitUpgrades(p);
   else
      Lith_PlayerReinitUpgrades(p);

   //
   // Static data

   if(!p->staticinit)
   {
      p->log("> Lithium " Lith_Version " :: Compiled %S", __DATE__);

      if(world.dbgLevel) {
         p->logH("> player is %u bytes long!", sizeof(struct player) * 4);
         p->logH("> strnull is \"%S\"", strnull);
         PrintDmonAllocSize(p);
      } else {
         p->logH("> Press \"%jS\" to open the menu.", "lith_k_opencbi");
      }

      p->deliverMail("Intro");

      p->staticinit = true;
   }

   if(world.dbgItems)
   {
      for(int i = weapon_min; i < weapon_max; i++) {
         weaponinfo_t const *info = &weaponinfo[i];
         if(info->classname != null && info->pclass & p->pclass && !(info->flags & wf_magic))
            InvGive(info->classname, 1);
      }
   }
}

//
// Lith_PlayerUpdateStats
//
[[__call("StkCall")]]
void Lith_PlayerUpdateStats(struct player *p)
{
   fixed boost = 1 + p->jumpboost;

   if(p->frozen     != p->old.frozen)
      ACS_SetPlayerProperty(0, p->frozen > 0, PROP_TOTALLYFROZEN);

   if(p->speedmul   != p->old.speedmul)
      ACS_SetActorPropertyFixed(0, APROP_Speed, 0.7 + p->speedmul);

   if(p->jumpboost  != p->old.jumpboost)
      ACS_SetActorPropertyFixed(0, APROP_JumpZ, p->jumpheight * boost);
}

// EOF
